#include <iostream>

#include "GurobiSolver.h"

#ifdef USE_GUROBI_NATIVE

using namespace std;


GurobiSolver::GurobiSolver()
: Solver()
, env_(0)
{
}

void GurobiSolver::resetModel() {
  if (model_!=0) {
    deleteModelAndEnv();
  }
  try {
    env_ = new GRBEnv();
    model_ = new SolverModel(*env_);

  } catch (GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
    
  } catch (...) {
    // somehow not printed
    cerr << "ERROR: Gurobi exception catched at calling new SolverModel()" << endl;
  }
  if (model_==0) {
    // somehow not printed
    cerr << "ERROR: Could not initialize Gurobi problem." << endl;
    cerr << "  Possible cause is that another Gurobi is already running," << endl;
    cerr << "  or that your license server is not correctly set up." << endl;
    cerr << "Quitting." << endl;
    assert(false);
    exit(0);
  }
}

const SolverVar & GurobiSolver::addLpVar(
  double lo, double hi, double objCoef, const std::string & name) {
  varVector_.push_back(model_->addVar(
    lo, hi, objCoef, GRB_CONTINUOUS, lpConvert(name).c_str()));
  model_->update();
  return varVector_.back();
}

const SolverVar & GurobiSolver::addIntVar(
  int lo, int hi, double objCoef, const std::string & name) {
  varVector_.push_back(model_->addVar(
    lo, hi, objCoef, GRB_INTEGER, lpConvert(name).c_str()));
  model_->update();

  /*
	if (lo==0) {
		unsigned int varVectorSize = varVector_.size();
		SolverExpr nullExpr; // = 0;
		model_->addConstr(nullExpr, GRB_LESS_EQUAL, varVector_[varVectorSize-1], 
											lpConvert(name + "_xtraLbIs0"));
		model_->update();
	}
	
	if (hi==0) {
		unsigned int varVectorSize = varVector_.size();
		SolverExpr nullExpr; // = 0;
		model_->addConstr(nullExpr, GRB_GREATER_EQUAL, varVector_[varVectorSize-1], 
											lpConvert(name + "_xtraUbIs0"));
		model_->update();
	}
  */

  return varVector_.back();
}

const SolverVar & GurobiSolver::addBinVar(
  double objCoef, const std::string & name) {
  varVector_.push_back(model_->addVar(
    0, 1, objCoef, GRB_BINARY, lpConvert(name).c_str()));
  model_->update();
  return varVector_.back();
}

const SolverExpr & GurobiSolver::addExpr(
  const SolverExpr & expr, const std::string & name) {
  exprVector_.push_back(expr);
  model_->update();
  return exprVector_.back();
}

const SolverConstr & GurobiSolver::addConstr(
  const SolverVar & lhs, const std::string & comp, const SolverVar & rhs, 
  const std::string & name) {
  
  char ch = compStringToChar(comp,
    "addConstr(SolverVar &, const string &, SolverVar &)");
  constrVector_.push_back(model_->addConstr(lhs, ch, rhs, lpConvert(name)));
  model_->update();
  return constrVector_.back();
}

const SolverConstr & GurobiSolver::addConstr(
  const SolverExpr & lhs, const std::string & comp, const SolverVar & rhs, 
  const std::string & name) {
  
  char ch = compStringToChar(comp,
                             "addConstr(SolverExpr &, const string &, SolverVar &)");
  constrVector_.push_back(model_->addConstr(lhs, ch, rhs, lpConvert(name)));
  model_->update();
  return constrVector_.back();
}

const SolverConstr & GurobiSolver::addConstr(
  const SolverExpr & lhs, const std::string & comp, const SolverExpr & rhs, 
  const std::string & name) {

  char ch = compStringToChar(comp, 
    "addConstr(SolverExpr &, const string &, SolverExpr &)");
  constrVector_.push_back(model_->addConstr(lhs, ch, rhs, lpConvert(name)));
  model_->update();
  return constrVector_.back();
}

// static
char GurobiSolver::compStringToChar(
  const string & comp, const string & funcName) {
  char ch = 'x';
  if (comp=="==") {
    ch = GRB_EQUAL;
  } else if (comp=="<=") {
    ch = GRB_LESS_EQUAL;
  } else if (comp==">=") {
    ch = GRB_GREATER_EQUAL;
  } else {
    cerr << "ERROR: In GurobiSolver::" << funcName << endl;
    cerr << "  Wrong comparator argument string " << comp << " supplied." << endl;
    cerr << "  Should be '==', '<=' or '>='." << endl;
    assert(false);
    exit(0);
  }
  return ch;
}

void GurobiSolver::exportModelAsLpFile(const string & fileNamePrefix) const {
  cout << "exportModelAsLpFile" << endl;
  const_cast<SolverModel *>(model_)->
    write((fileNamePrefix + ".lp").c_str()); 
    // does not add lp extension by itself
}

void GurobiSolver::exportModelAsMpsFile(const string & fileNamePrefix) const {
  cout << "exportModelAsMpsFile" << endl;
  const_cast<SolverModel *>(model_)->
    write((fileNamePrefix + ".mps").c_str());
  // Adds .mat extension by default,
  // but we avoid .mat extension 'coz Windows 
  // interprets this as Microsoft Access Table 
}

void GurobiSolver::setMinimize() {
  model_->set(GRB_IntAttr_ModelSense, 1); // minimize (is in fact default)
}

void GurobiSolver::setMaximize() {
  model_->set(GRB_IntAttr_ModelSense, 0); // maximize (is not default)
}

bool GurobiSolver::solve(double gap) {
  cout << "In GurobiSolver::solve()" << endl;

  // maybe set mesage level here, as in XpressSolver
  
  solved_ = false;
  try {
    model_->optimize();
    int status = model_->get(GRB_IntAttr_Status);   
    cout << "Optimization was stopped with status " << status << endl;  
    if (status == GRB_OPTIMAL) {
      solved_ = true;
      cout << "The model is solved optimally." << endl;
      cout << "Objective: " 
           //<< model_->get(GRB_DoubleAttr_ObjVal) 
           << getObjVal() 
           << endl;
    } else {
      solved_ = false;
      cout << "The model cannot be solved ";
      if (status == GRB_UNBOUNDED) {
        cout << "because it is GRB_UNBOUNDED" << endl;
      } else if (status == GRB_INF_OR_UNBD) { 
        cout << "because it is GRB_INF_OR_UNBD" << endl;
      } else if (status == GRB_INFEASIBLE) {
        cout << "because it is GRB_INFEASIBLE" << endl;
      } else {
        cout << "for unknown reasons." << endl;
      }
      calcAnIIS();
      reportAnIISTo(cout);
    }
  } catch (GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch (...) {
    cerr << "ERROR: Gurobi exception catched at calling new GurobiSolver::solve()" 
         << endl;
  }
    
  return solved_;
}

double GurobiSolver::getValueOf(const SolverExpr & expr) const {
  double value = 0;
  value += expr.getConstant();
  unsigned int nTerms = expr.size();
  for (unsigned int i=0; i<nTerms; i++) {
    value += expr.getCoeff(i) * expr.getVar(i).get(GRB_DoubleAttr_X);
  }
  return value;
}

double GurobiSolver::getValueOf(const SolverVar & var) const {
  return var.get(GRB_DoubleAttr_X);
}

double GurobiSolver::getObjVal() const {
  return model_->get(GRB_DoubleAttr_ObjVal);
}

//const SolverSos & 
void GurobiSolver::addSos1SolverSpecific(
  const string & name,
  const vector<SolverVar> & vars, const vector<double> & weights) {

  unsigned int nVars   = (unsigned int)vars.size();
  unsigned int nWeights = (unsigned int)weights.size();
  assert(nVars == nWeights);

  //GRBVar sosv1[]  = {x[0], x[1]};
  SolverVar * sosVars = new SolverVar[nVars];
  assert(sosVars!=0);
  for (unsigned int v=0; v<nVars; v++) {
    sosVars[v] = (vars[v]); // removed pointers
  }

  //double soswt1[] = {1, 2};
  double * sosWeights = new double[nWeights];
  assert(sosWeights!=0);
  for (unsigned int w=0; w<nWeights; w++) {
    sosWeights[w] = weights[w];
  }

  model_->update();

  //model_->addSOS(GRB.SOS_TYPE1, [x, y, z], [1, 2, 4]);
  //const SolverSos & sos = 
#ifdef USE_SOS_VECTOR  
  sosVector_.push_back(model_->addSOS(sosVars, sosWeights, nVars, GRB_SOS_TYPE1));
#else
  model_->addSOS(sosVars, sosWeights, nVars, GRB_SOS_TYPE1);
#endif
  // sos should be steered to start branching for low values
  
  model_->update();

  // Can I delete these here? 
  // Did Gurobi copy all necesary data for it to work?
  delete [] sosVars;
  delete [] sosWeights;

  //return sosVector_.back();
}


string GurobiSolver::getName(const SolverVar & var) const {
  return var.get(GRB_StringAttr_VarName);
}

double GurobiSolver::getLowerBound(const SolverVar & var) const {
  return var.get(GRB_DoubleAttr_LB);
}

double GurobiSolver::getUpperBound(const SolverVar & var) const {
  return var.get(GRB_DoubleAttr_UB);
}


unsigned int GurobiSolver::getNumberOfRows() const {
  return model_->get(GRB_IntAttr_NumConstrs);
}

unsigned int GurobiSolver::getNumberOfColumns() const {
  return model_->get(GRB_IntAttr_NumVars);
}

unsigned int GurobiSolver::getNumberOfNonZeroes() const {
  return model_->get(GRB_IntAttr_NumNZs);
}


unsigned int GurobiSolver::getNumberOfSets() const {
  return model_->get(GRB_IntAttr_NumSOS);
}

unsigned int GurobiSolver::getNumberOfSetMembers() const {
  //return SETMEMBERS;
  assert(false);
  return 0;
}


void GurobiSolver::deleteModelAndEnv() {
  delete model_;
  model_ = 0;  
  delete env_;
  env_ = 0;
}

void GurobiSolver::calcAnIIS() const {
  model_->computeIIS();
}

void GurobiSolver::reportAnIISTo(ostream & str) const {
  str << "The following constraint(s) cannot be satisfied:" << endl;
  GRBConstr* c = model_->getConstrs();
  for (int i = 0; i < model_->get(GRB_IntAttr_NumConstrs); ++i) {
    if (c[i].get(GRB_IntAttr_IISConstr) == 1) {
      cout << c[i].get(GRB_StringAttr_ConstrName) << endl;
    }
  }
  delete [] c;
}


double GurobiSolver::getInfinity() const {
  return GRB_INFINITY;
}


GurobiSolver::~GurobiSolver() {
  deleteModelAndEnv();
}

#endif // USE_GUROBI_NATIVE
