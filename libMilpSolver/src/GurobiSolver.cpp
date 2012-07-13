#include <iostream>
#include <cstdlib>
#include <sstream>

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
    exit(0);    
  } catch (...) {
    // somehow not printed
    cerr << "ERROR: Gurobi exception catched at calling new SolverModel()"
    << endl;
    exit(0);
  }
  if (model_==0) {
    // somehow not printed
    cerr << "ERROR: Could not initialize Gurobi problem." << endl;
    cerr << "  Possible cause is that another Gurobi is already running," 
    << endl;
    cerr << "  or that your license server is not correctly set up." << endl;
    cerr << "Quitting." << endl;
    assert(false);
    exit(0);
  }
	
	resetModelNullOneExpressions();
}

const SolverVar & GurobiSolver::addLpVar(
  double lo, double hi, double objCoef, const std::string & name,
	bool doUpdate) {
  varVector_.push_back(model_->addVar(
    lo, hi, objCoef, GRB_CONTINUOUS, lpConvert(name).
                                     substr(0, STR_MAX_LEN).c_str()));
	if (doUpdate) {
    model_->update();
	}
  return varVector_.back();
}

const SolverVar & GurobiSolver::addIntVar(
  int lo, int hi, double objCoef, const std::string & name, bool doUpdate) {
  varVector_.push_back(model_->addVar(
    lo, hi, objCoef, GRB_INTEGER, lpConvert(name).
                                  substr(0, STR_MAX_LEN).c_str()));
	if (doUpdate) {
    model_->update();
	}
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
  double objCoef, const std::string & name, bool doUpdate) {
  varVector_.push_back(model_->addVar(
    0, 1, objCoef, GRB_BINARY, lpConvert(name).
                               substr(0, STR_MAX_LEN).c_str()));
	if (doUpdate) {
    model_->update();
	}
  return varVector_.back();
}

const SolverExpr & GurobiSolver::addExpr(
  const SolverExpr & expr, const std::string & name, bool doUpdate) {
  exprVector_.push_back(expr);
	if (doUpdate) {
    model_->update();
	}
  return exprVector_.back();
}

const SolverConstr & GurobiSolver::addConstr(
  const SolverVar & lhs, const std::string & comp, const SolverVar & rhs, 
  const std::string & name, bool doUpdate) {
  
  char ch = compStringToChar(comp,
    "addConstr(SolverVar &, const string &, SolverVar &)");
  constrVector_.push_back(model_->addConstr(lhs, ch, rhs, 
                                            lpConvert(name).
                                            substr(0, STR_MAX_LEN)));
	if (doUpdate) {
    model_->update();
	}
  return constrVector_.back();
}

const SolverConstr & GurobiSolver::addConstr(
  const SolverExpr & lhs, const std::string & comp, const SolverVar & rhs, 
  const std::string & name, bool doUpdate) {
  
  char ch = 
  compStringToChar(comp,
                   "addConstr(SolverExpr &, const string &, SolverVar &)");
  constrVector_.push_back(model_->addConstr(lhs, ch, rhs, 
                                            lpConvert(name).
                                            substr(0, STR_MAX_LEN)));
	if (doUpdate) {
    model_->update();
	}
  return constrVector_.back();
}

const SolverConstr & GurobiSolver::addConstr(
  const SolverExpr & lhs, const std::string & comp, const SolverExpr & rhs, 
  const std::string & name, bool doUpdate) {

  char ch = compStringToChar(comp, 
    "addConstr(SolverExpr &, const string &, SolverExpr &)");
  constrVector_.push_back(model_->addConstr(lhs, ch, rhs, 
                                            lpConvert(name).
                                            substr(0, STR_MAX_LEN)));
	if (doUpdate) {
    model_->update();
	}
  return constrVector_.back();
}

//////////// fast constraint addition without pushing onto vector ////////////

void GurobiSolver::fastAddConstr(const SolverVar & lhs, 
                                 const std::string & comp, 
                                 const SolverVar & rhs, 
                                 const std::string & name) {
  char ch = 
  compStringToChar(comp,
                   "fastAddConstr(SolverVar &, const string &, SolverVar &)");
  model_->addConstr(lhs, ch, rhs, lpConvert(name).
                    substr(0, STR_MAX_LEN));
}

void GurobiSolver::fastAddConstr(const SolverExpr & lhs, 
                                 const std::string & comp, 
                                 const SolverVar & rhs, 
                                 const std::string & name) {
  char ch = 
  compStringToChar(comp,
                   "fastAddConstr(SolverExpr &, const string &, SolverVar &)");
  model_->addConstr(lhs, ch, rhs, lpConvert(name).
                    substr(0, STR_MAX_LEN));
}

void GurobiSolver::fastAddConstr(const SolverExpr & lhs, 
                                 const std::string & comp, 
                                 const SolverExpr & rhs, 
                                 const std::string & name) {
  char ch = 
  compStringToChar(comp, 
                   "addConstr(SolverExpr &, const string &, SolverExpr &)");
  model_->addConstr(lhs, ch, rhs, lpConvert(name).
                    substr(0, STR_MAX_LEN));
}

///////////////////////////////////////////////////////////////////////////////



// Specifically for Gurobi really
void GurobiSolver::update() {
  //cout << "GurobiSolver::update()" << endl;
  model_->update();
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
    cerr << "  Wrong comparator argument string " << comp << " supplied." 
    << endl;
    cerr << "  Should be '==', '<=' or '>='." << endl;
    assert(false);
    exit(0);
  }
  return ch;
}

void GurobiSolver::exportModelAsLpFile(const string & fileNamePrefix) const {
  const_cast<SolverModel *>(model_)->
    write((fileNamePrefix + ".lp").c_str()); 
    // does not add lp extension by itself
}

void GurobiSolver::exportModelAsMpsFile(const string & fileNamePrefix) const {
  const_cast<SolverModel *>(model_)->
    write((fileNamePrefix + ".mps").c_str());
  // Adds .mat extension by default,
  // but we avoid .mat extension 'coz Windows 
  // interprets this as Microsoft Access Table 
}

void GurobiSolver::exportModelAsRewFile(const string & fileNamePrefix) const {
  const_cast<SolverModel *>(model_)->
  write((fileNamePrefix + ".rew").c_str());
  // Adds .mat extension by default,
  // but we avoid .mat extension 'coz Windows 
  // interprets this as Microsoft Access Table 
}

void GurobiSolver::setMinimize() {
	model_->set(GRB_StringAttr_ModelName, "GurobiModelMin");
  //model_->set(GRB_IntAttr_ModelSense, 1); // minimize (is in fact default)
  //model_->setIntAttr("ModelSense", 1); // minimize (is in fact default)
	model_->update();
}

void GurobiSolver::setMaximize() {
	model_->set(GRB_StringAttr_ModelName, "GurobiModelMax");
  //model_->set(GRB_IntAttr_ModelSense, 0); // maximize (is not default)
  model_->set(GRB_IntAttr_ModelSense, -1); // maximize (is not default)
	model_->update();
}

void GurobiSolver::setStartValueOf(SolverVar & var, 
                                   double startValue) const {
  var.set(GRB_DoubleAttr_Start, startValue);
}

bool GurobiSolver::solve(double gap, int nThreads) {
  cout << "In GurobiSolver::solve()" << endl;

  // maybe set mesage level here, as in XpressSolver
  //model_->set(GRB_DBL_PAR_MIPGAP, gap); // C
  model_->getEnv().set(GRB_DoubleParam_MIPGap, gap); // C++

  // FIXME: The settings below should be moved to the Solver level
  // so that other solvers, like: CPLEX and XPRESS act the same
  
  // set mem for node storage to this value. Beyond, swapping to disk happens.
  model_->getEnv().set(GRB_DoubleParam_NodefileStart, 4.0); // C++
  // not sure about the above
  
  //model_->getEnv().set(GRB_IntParam_Presolve, 2.0); // C++
  // 2.0=Aggressive constraint aggregation
  // Hoping this reduces the number or/and range of d integer params.

  // Greg Glockners suggestion
  //setMipFocus(1);
  model_->getEnv().set(GRB_DoubleParam_Heuristics, 0.10); // C++
  
  //model_->getEnv().set(GRB_IntParam_PumpPasses, 50); // C++
  //model_->getEnv().set(GRB_IntParam_MinRelNodes, 500); // C++
  //model_->getEnv().set(GRB_IntParam_ZeroObjNodes, 500); // C++ 
  // GRB_IntParam_ZeroObjNodes present from gurobi 4.6.0 onwards
    
  setNThreads(nThreads);
  
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
    cerr 
    << "ERROR: Gurobi exception catched at calling new GurobiSolver::solve()" 
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

double GurobiSolver::getObjBound() const {
  return model_->get(GRB_DoubleAttr_ObjBound);
}

double GurobiSolver::getMipGap() const {
  double bound = model_->get(GRB_DoubleAttr_ObjBound);
  double value = model_->get(GRB_DoubleAttr_ObjVal);
  double mipGap = 1 - (bound / value);
  return mipGap;
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
  sosVector_.push_back(model_->addSOS(sosVars, sosWeights, 
                                      nVars, GRB_SOS_TYPE1));
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


void GurobiSolver::setLowerBound(SolverVar & var, double value) {
	var.set(GRB_DoubleAttr_LB, value);
}

void GurobiSolver::setUpperBound(SolverVar & var, double value) {
	var.set(GRB_DoubleAttr_UB, value);
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

void GurobiSolver::setNThreads(int nThreads) {
  model_->getEnv().set(GRB_IntParam_Threads, nThreads); // C++
}

void GurobiSolver::setDeterministic() {
	unsigned int method = 4;
  model_->getEnv().set(GRB_IntParam_Method, method); // C++
}

void GurobiSolver::setMipFocus(unsigned int value) {
/*Controls the focus of the MIP solver. The default value tries to 
   strike a balance between finding good feasible solutions and 
   proving optimality. A value of 1 shifts the focus towards finding 
   feasible solutions. A value of 2 shifts the focus towards proving optimality.
   A value of 3 focuses almost entirely on moving the best objective bound.
 */
  assert(0 <= value);
	assert(value <= 3);
  model_->getEnv().set(GRB_IntParam_MIPFocus, value); // C++
}

void GurobiSolver::setSymmetryDetectionLevel(int level) {
	assert(-1 <= level);
	assert(level <= 2);
  model_->getEnv().set(GRB_IntParam_Symmetry, level); // C++	
}

void GurobiSolver::improveStartGap(double gap) {
	assert(0.0 <= gap);
	assert(gap <= GRB_INFINITY);
  model_->getEnv().set(GRB_DoubleParam_ImproveStartGap, gap); // C++	
}

void GurobiSolver::improveStartTime(double time) {
	assert(0.0 <= time);
	assert(time <= GRB_INFINITY);
  model_->getEnv().set(GRB_DoubleParam_ImproveStartTime, time); // C++	
}

// 0.05 is default
void GurobiSolver::setHeuristics(double level) {
	assert(0.0 <= level);
	assert(level <= 1.0);
  model_->getEnv().set(GRB_DoubleParam_Heuristics, level); // C++		
}

void GurobiSolver::setVarBranch(int value) {
	assert(-1 <= value);
	assert(value <= 3);
  model_->getEnv().set(GRB_IntParam_VarBranch, value); // C++		
}

////////////////// Cuts Control ///////////////
void GurobiSolver::setCutsLevel(int level)  {
	assert(-1 <= level);
	assert(level <= 3);
  model_->getEnv().set(GRB_IntParam_Cuts, level); // C++		
}

void GurobiSolver::setCliqueCutsLevel(int level) {
	assert(-1 <= level);
	assert(level <= 2);
  model_->getEnv().set(GRB_IntParam_CliqueCuts, level); // C++		
}

void GurobiSolver::setCoverCutsLevel(int level) {
	assert(-1 <= level);
	assert(level <= 2);
  model_->getEnv().set(GRB_IntParam_CoverCuts, level); // C++		
}

void GurobiSolver::setFlowCoverCutsLevel(int level)  {
	assert(-1 <= level);
	assert(level <= 2);
  model_->getEnv().set(GRB_IntParam_FlowCoverCuts, level); // C++		
}

void GurobiSolver::setFlowPathCutsLevel(int level) {
	assert(-1 <= level);
	assert(level <= 2);
  model_->getEnv().set(GRB_IntParam_FlowPathCuts, level); // C++		
}

// http://www.gurobi.com/doc/40/refman/node572.html
void GurobiSolver::setBranchingMethod(int method) {
  assert(-1 <= method);
  assert(method <= 3);
  model_->getEnv().set(GRB_IntParam_VarBranch, method); // C++		 
}

////////////////// End Cuts Control ///////////////

string GurobiSolver::getVersionString() const {
  int major     = GRB_VERSION_MAJOR;
  int minor     = GRB_VERSION_MINOR;
  int technical = GRB_VERSION_TECHNICAL;
  stringstream ss;
  ss << major << "." << minor << "." << technical;
  string versionString = ss.str();
  return versionString;
}

void GurobiSolver::setIntFeasTol(double value) {
  model_->getEnv().set(GRB_DoubleParam_IntFeasTol, value); // C++		     
}

GurobiSolver::~GurobiSolver() {
  deleteModelAndEnv();
}

#endif // USE_GUROBI_NATIVE
