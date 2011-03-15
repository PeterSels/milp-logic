#include <iostream>
#include <assert.h>

#include "XpressSolver.h"

#ifdef USE_XPRESS_NATIVE


#include "xprb_cpp.h"
#include "xprb.h"
#include "xprs.h"

using namespace std;
using namespace dashoptimization;

XpressSolver::XpressSolver() 
: Solver()
{
}

void XpressSolver::resetModel() {
  if (model_!=0) {
    delete model_;
    model_ = 0;
  }
  try {
    model_ = new SolverModel();

  } catch (BclException & e) {
    cout << "Error code = " << e.code << endl;
    cout << e.msg << endl;
    
  } catch (...) {
    // somehow not printed
    cerr << "ERROR: Xpress exception catched at calling new SolverModel()" << endl;
  }
  if (model_==0) {
    // somehow not printed
    cerr << "ERROR: Could not initialize Xpress problem." << endl;
    cerr << "  Possible cause is that another Xpress is already running," << endl;
    cerr << "  or that your license server is not correctly set up." << endl;
    cerr << "Quitting." << endl;
    assert(false);
    exit(0);
  }
  objFunction_ = 0;
  // Set the objective function
  // special to xpress compared to Gurobi:
  model_->setObj(objFunction_); 

	resetModelNullOneExpressions();
}

const SolverVar & XpressSolver::addLpVar(
  double lo, double hi, double objCoef, const std::string & name,
  bool doUpdate) {
	
	(void)doUpdate;
  SolverVar & var = model_->newVar(
    lpConvert(name).c_str(), XPRB_PL, lo, hi);
  varVector_.push_back(var);
  // explicit store objCoef in obj function
  objFunction_ += objCoef * var;
  model_->setObj(objFunction_);
  return var;
}

const SolverVar & XpressSolver::addIntVar(
  int lo, int hi, double objCoef, const std::string & name,
  bool doUpdate) {
	
	(void)doUpdate;
  SolverVar & var = model_->newVar(
    lpConvert(name).c_str(), XPRB_UI, lo, hi);
  varVector_.push_back(var);
  // explicit store objCoef in obj function
  objFunction_ += objCoef * var;
  model_->setObj(objFunction_); 
  return var;
}

const SolverVar & XpressSolver::addBinVar(
  double objCoef, const std::string & name, bool doUpdate) {
	
	(void)doUpdate;
  SolverVar & var = model_->newVar(
    lpConvert(name).c_str(), XPRB_BV);
  varVector_.push_back(var);
  // explicit store objCoef in obj function
  objFunction_ += objCoef * var;
  model_->setObj(objFunction_); 
  return var;
}

const SolverExpr & XpressSolver::addExpr(
  const SolverExpr & expr, const std::string & name) {
  //exprVector_.push_back(model_->addExpr(expr, name.str());
  exprVector_.push_back(expr);
  return exprVector_.back();
}

const SolverConstr & XpressSolver::addConstr(
  const SolverVar & lhs, const std::string & comp, const SolverVar & rhs, 
  const std::string & name) {
  
  if (comp=="==") {
    constrVector_.push_back(model_->newCtr(
      lpConvert(name).c_str(), lhs == rhs));
  }  else if (comp=="<=") {
    constrVector_.push_back(model_->newCtr(
      lpConvert(name).c_str(), lhs <= rhs));
  } else if (comp==">=") {
    constrVector_.push_back(model_->newCtr(
      lpConvert(name).c_str(), lhs >= rhs));
  } else {
    cerr << "ERROR: In XpressSolver::addConstr(...)" << endl;
    cerr << "  Wrong comparator argument string " << comp << " supplied." << endl;
    cerr << "  Should be '==', '<=' or '>='." << endl;
    assert(false);
    exit(0);
  }
  return constrVector_.back();
}

const SolverConstr & XpressSolver::addConstr(
  const SolverExpr & lhs, const std::string & comp, const SolverVar & rhs, 
  const std::string & name) {
  
  if (comp=="==") {
    constrVector_.push_back(model_->newCtr(
      lpConvert(name).c_str(), lhs == rhs));
  }  else if (comp=="<=") {
    constrVector_.push_back(model_->newCtr(
      lpConvert(name).c_str(), lhs <= rhs));
  } else if (comp==">=") {
    constrVector_.push_back(model_->newCtr(
      lpConvert(name).c_str(), lhs >= rhs));
  } else {
    cerr << "ERROR: In XpressSolver::addConstr(...)" << endl;
    cerr << "  Wrong comparator argument string " << comp << " supplied." << endl;
    cerr << "  Should be '==', '<=' or '>='." << endl;
    assert(false);
    exit(0);
  }
  return constrVector_.back();
}

const SolverConstr & XpressSolver::addConstr(
  const SolverExpr & lhs, const std::string & comp, const SolverExpr & rhs, 
  const std::string & name) {

  if (comp=="==") {
    constrVector_.push_back(model_->newCtr(
      lpConvert(name).c_str(), lhs == rhs));
  }  else if (comp=="<=") {
    constrVector_.push_back(model_->newCtr(
      lpConvert(name).c_str(), lhs <= rhs));
  } else if (comp==">=") {
    constrVector_.push_back(model_->newCtr(
      lpConvert(name).c_str(), lhs >= rhs));
  } else {
    cerr << "ERROR: In XpressSolver::addConstr(...)" << endl;
    cerr << "  Wrong comparator argument string " << comp << " supplied." << endl;
    cerr << "  Should be '==', '<=' or '>='." << endl;
    assert(false);
    exit(0);
  }
  return constrVector_.back();
}

//const SolverSos & 
void XpressSolver::addSos1SolverSpecific(
  const string & name,
  const vector<SolverVar> & vars, 
  const vector<double> & weights) {

  unsigned int nVars    = (unsigned int)vars.size();
  unsigned int nWeights = (unsigned int)weights.size();
  assert(nVars == nWeights);

  SolverExpr linExpr = 0;
  for (unsigned int v=0; v<nVars; v++) {
    linExpr += weights[v] * (vars[v]); // removed pointer use
  }

  string lpName = lpConvert(name);

#ifdef USE_SOS_VECTOR
  sosVector_.push_back(
    model_->newSos(lpName.c_str(), XPRB_S1, linExpr)
  );

  bool valid = sosVector_.back().isValid();
  assert(valid);
  sosVector_.back().setDir(XPRB_DN); // first branch down on sets
  sosVector_.back().print();

  SolverSos & sosBack = model_->getSosByName(lpName.c_str());
  valid = sosBack.isValid();
  assert(valid);
  //return sosVector_.back();
#else
  model_->newSos(lpName.c_str(), XPRB_S1, linExpr).setDir(XPRB_DN);
  // not by (local) reference because we need to return it.
#endif // USE_SOS_VECTOR
}

void XpressSolver::update() {
  // nothing to do
}


void XpressSolver::exportModelAsLpFile(const string & fileNamePrefix) const {
  cout << "exportModelAsLpFile" << endl;
  // For an LP format matrix use the following:
  const_cast<SolverModel *>(model_)->
    exportProb(XPRB_LP, fileNamePrefix.c_str()); // adds lp extension by itself
}

void XpressSolver::exportModelAsMpsFile(const string & fileNamePrefix) const {
  cout << "exportModelAsMpsFile" << endl;
  // For an MPS format matrix use the following:
  const_cast<SolverModel *>(model_)->
    exportProb(XPRB_MPS, (fileNamePrefix + ".mps").c_str());
  // Adds .mat extension by default,
  // but we avoid .mat extension 'coz Windows 
  // interprets this as Microsoft Access Table 
}

void XpressSolver::setMinimize() {
  model_->setSense(XPRB_MINIM);
}

void XpressSolver::setMaximize() {
  model_->setSense(XPRB_MAXIM);
}

bool XpressSolver::solve(double gap) {
  cout << "In XpressSolver::solve()" << endl;

  /* xpress:
  0 no messages printed;
  1 error messages only printed;
  2 warnings and errors printed;
  3 warnings, errors, and Optimizer log printed (default);
  4 all messages printed.
  */  
  model_->setMsgLevel(3);

  solved_ = false;
  model_->solve("g"); // Solve the problem as MIP

  int statmip = model_->getMIPStat(); // Get the MIP problem status
  if (statmip == XPRB_MIP_OPTIMAL) {
    cout << "The model is solved optimally" << endl;
    solved_ = true;
  } else {
    if (statmip == XPRB_MIP_SOLUTION) {
      // An integer solution has been found
      cout << "WARNING: However, this solution is NOT optimal!" << endl;
      solved_ = true;
    } else {
      cout << "WARNING: No integer solution found!" << endl;
      solved_ = false;
    }
  }
  return solved_;
}

double XpressSolver::getValueOf(const SolverExpr & expr) const {
  return expr.getSol();
}

double XpressSolver::getValueOf(const SolverVar & var) const {
  return var.getSol();
}

string XpressSolver::getName(const SolverVar & var) const {
  return var.getName();
}

double XpressSolver::getLowerBound(const SolverVar & var) const {
  return var.getLB();
}

double XpressSolver::getUpperBound(const SolverVar & var) const {
  return var.getUB();
}

void XpressSolver::setLowerBound(const SolverVar & var, double value) const {
  var.setLB(value);
}

void XpressSolver::setUpperBound(const SolverVar & var, double value) const {
  var.getUB(value);
}

double XpressSolver::getObjVal() const {
  return model_->getObjVal();
}


unsigned int XpressSolver::getNumberOfRows() const {
  int rows;
  xo_prob_struct * opt_prob = model_->getXPRSprob();
  XPRSgetintattrib(opt_prob, XPRS_ORIGINALROWS, &rows);  
  assert(rows >= 0);
  return (unsigned int)rows;
}

unsigned int XpressSolver::getNumberOfColumns() const {
  int cols;
  ///model_->getIntAttrib(XPRS.ORIGINALCOLS);
  xo_prob_struct * opt_prob = model_->getXPRSprob();
  XPRSgetintattrib(opt_prob, XPRS_ORIGINALCOLS, &cols);  
  assert(cols >= 0);
  return (unsigned int)cols;
}

unsigned int XpressSolver::getNumberOfNonZeroes() const {
  int nonZeroes;
  /*
  xo_prob_struct * opt_prob = model_->getXPRSprob();
  XPRSgetintattrib(opt_prob, XPRS_NZS, &nonZeroes);  
  assert(nonZeroes >= 0);
  */
  nonZeroes = 0; // FIXME
  return (unsigned int)nonZeroes;
}


unsigned int XpressSolver::getNumberOfSets() const {
  // FIXME
  return 0;
}

unsigned int XpressSolver::getNumberOfSetMembers() const {
  // FIXME
  return 0;
}

void XpressSolver::calcAnIIS() const {
  cout << "ERROR: calcAnIIS: unimplemented still." << endl;
  assert(false);
}

void XpressSolver::reportAnIISTo(ostream & ostr) const {
  ostr << "ERROR: reportAnIIS: unimplemented still." << endl;
  assert(false);
}


double XpressSolver::getInfinity() const {
  return XPRB_INFINITY;
}


XpressSolver::~XpressSolver() {
  delete model_;
  model_ = 0;  
}

#endif // USE_XPRESS_NATIVE
