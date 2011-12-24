#include <iostream>
#include <time.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
  // for Sleep in seconds
#else
#include <unistd.h>
  // for sleep in seconds
#endif

#include "HasSolver.h"

#ifdef USE_CPLEX_NATIVE

#include "CplexSolver.h"

using namespace std;

static const char* message[]={ 
	  "00 : OK Optimal Solution found", 
	  "01 : Error No Server found", 
	  "02 : Error No Token got within Time Limit of ", 
	  "03 : Error No Solution found within Time Limit of ", 
	  "04 : Error Optimization failed", 
	  "05 : Error Abort by User", 
	  "06 : Error No License file found  ", 
	  "07 : Error Other Error "
}; 

enum { 
  MESSAGE_OK, 
  MESSAGE_NO_SERVER, 
  MESSAGE_NO_TOKEN, 
  MESSAGE_TIME_OUT, 
  MESSAGE_ERROR_OPT, 
  MESSAGE_ABORT_USER, 
  MESSAGE_NO_LICENSE, 
  MESSAGE_OTHER
}; 

IloEnv * global_env_; // same as env_, but Cplex often needs 
// it in Ilo object construction
IloCplex * global_cplex_;

CplexSolver::CplexSolver(unsigned int maxGetLicenseSeconds, 
                         unsigned int maxSolveSeconds) 
: Solver()
, maxGetLicenseSeconds_(maxGetLicenseSeconds)
, maxSolveSeconds_(maxSolveSeconds)
{
  assert(maxGetLicenseSeconds_ >= 0);
  assert(maxSolveSeconds_ >= 0);
}

void CplexSolver::anyOSSleep(unsigned int nSeconds) {
#ifdef _WIN32
	Sleep(nSeconds);
#else
	sleep(nSeconds);
#endif
}

void CplexSolver::resetModel() {
  if (model_!=0) {
    deleteModelAndEnv();
  }
  try {
    global_env_ = env_ = new IloEnv();
    model_ = new SolverModel(*env_);

  } catch (IloCplex::Exception & e) {
    cout << "Error code = " << e.getStatus() << endl;
    //throw e;

    if (e.getStatus() == 32201) {
		  istringstream flux(e.getMessage());
      string str;
      int errorCode;
		  flux >> str >> str >> str >> str >> str >> errorCode;
		  switch(errorCode) {
		    case 16: // Licence file not found
			    throw string(message[MESSAGE_NO_LICENSE]);
			    break;
		    case 15:  // No Token 
			    //std::cout << "License Wait .... " << ++n  << std::endl;
          {
			      int nSeconds = (rand() * 2000 / RAND_MAX);
						anyOSSleep(nSeconds);
					}
			    break;
		    case 4: // No server
			    throw string(message[MESSAGE_NO_SERVER]);
			    break;
		    default:
			    throw string(e.getMessage());
		  }
    } else {
      throw string(e.getMessage());
    } 

  } catch (IloException & e) {
    cout << e.getMessage() << endl;

  } catch (string &str) {
    throw str;
    
  } catch (...) {
    // somehow not printed
    cout << "ERROR: Cplex exception catched at calling new SolverModel()" << endl;
  }
  if (model_==0) {
    // somehow not printed
    cout << "ERROR: Could not initialize Cplex problem." << endl;
    cout << "  Possible cause is that another Cplex is already running," << endl;
    cout << "  or that your license server is not correctly set up." << endl;
    cout << "Quitting." << endl;
    assert(false);
    exit(0);
  }
  objFunction_ = new SolverExpr(*global_env_);
  // Set the objective function
  // special to Cplex compared to Gurobi:
  //////////model_->add(objFunction_); 
	
	resetModelNullOneExpressions();
}

const SolverVar & CplexSolver::addLpVar(
  double lo, double hi, double objCoef, const std::string & name,
  bool doUpdate) {

	(void)doUpdate;
  IloNumVar var(*env_, lo, hi, ILOFLOAT);
  var.setName(lpConvert(name).c_str());
  varVector_.push_back(var);
  // explicit store objCoef in obj function
  *objFunction_ += objCoef * var;
  return varVector_.back();
}

const SolverVar & CplexSolver::addIntVar(
  int lo, int hi, double objCoef, const std::string & name,
  bool doUpdate) {

	(void)doUpdate;
  IloNumVar var(*env_, lo, hi, ILOINT);
  var.setName(lpConvert(name).c_str());
  varVector_.push_back(var);
  // explicit store objCoef in obj function
  *objFunction_ += objCoef * var;
  return varVector_.back();
}

const SolverVar & CplexSolver::addBinVar(
  double objCoef, const std::string & name, bool doUpdate) {
	
	(void)doUpdate;
  IloNumVar var(*env_, 0, 1, ILOBOOL);
  var.setName(lpConvert(name).c_str());
  varVector_.push_back(var);
  // explicit store objCoef in obj function
  *objFunction_ += objCoef * var;
  return varVector_.back();
}

const SolverExpr & CplexSolver::addExpr(
  const SolverExpr & expr, const std::string & name, bool doUpdate) {
  //exprVector_.push_back(model_->addExpr(expr, name.str());
  exprVector_.push_back(expr);
  return exprVector_.back();
}

const SolverConstr & CplexSolver::addConstr(
  const SolverVar & lhs, const std::string & comp, const SolverVar & rhs, 
  const std::string & name, bool doUpdate) {
  
  if (comp=="==") {
    IloRange constr(*env_, 0, lhs - rhs, 0);
    model_->add(constr);
    constr.setName(lpConvert(name).c_str());
    constrVector_.push_back(constr);
  }  else if (comp=="<=") {
    IloRange constr(*env_, -IloInfinity, lhs - rhs, 0);
    model_->add(constr);
    constr.setName(lpConvert(name).c_str());
    constrVector_.push_back(constr);
  } else if (comp==">=") {
    IloRange constr(*env_, 0, lhs - rhs, +IloInfinity);
    model_->add(constr);
    constr.setName(lpConvert(name).c_str());
    constrVector_.push_back(constr);
  } else {
    cout << "ERROR: In CplexSolver::addConstr(...)" << endl;
    cout << "  Wrong comparator argument string " << comp << " supplied." << endl;
    cout << "  Should be '==', '<=' or '>='." << endl;
    assert(false);
    exit(0);
  }
  return constrVector_.back();
}

const SolverConstr & CplexSolver::addConstr(
  const SolverExpr & lhs, const std::string & comp, const SolverVar & rhs, 
  const std::string & name, bool doUpdate) {
  
  if (comp=="==") {
    IloRange constr(*env_, 0, lhs - rhs, 0);
    model_->add(constr);
    constr.setName(lpConvert(name).c_str());
    constrVector_.push_back(constr);
  }  else if (comp=="<=") {
    IloRange constr(*env_, -IloInfinity, lhs - rhs, 0);
    model_->add(constr);
    constr.setName(lpConvert(name).c_str());
    constrVector_.push_back(constr);
  } else if (comp==">=") {
    IloRange constr(*env_, 0, lhs - rhs, +IloInfinity);
    model_->add(constr);
    constr.setName(lpConvert(name).c_str());
    constrVector_.push_back(constr);
  } else {
    cout << "ERROR: In CplexSolver::addConstr(...)" << endl;
    cout << "  Wrong comparator argument string " << comp << " supplied." << endl;
    cout << "  Should be '==', '<=' or '>='." << endl;
    assert(false);
    exit(0);
  }
  return constrVector_.back();
}

const SolverConstr & CplexSolver::addConstr(
  const SolverExpr & lhs, const std::string & comp, const SolverExpr & rhs, 
  const std::string & name, bool doUpdate) {

  if (comp=="==") {
    IloRange constr(*env_, 0, lhs - rhs, 0);
    model_->add(constr);
    constr.setName(lpConvert(name).c_str());
    constrVector_.push_back(constr);
  }  else if (comp=="<=") {
    IloRange constr(*env_, -IloInfinity, lhs - rhs, 0);
    model_->add(constr);
    constr.setName(lpConvert(name).c_str());
    constrVector_.push_back(constr);
  } else if (comp==">=") {
    IloRange constr(*env_, 0, lhs - rhs, +IloInfinity);
    model_->add(constr);
    constr.setName(lpConvert(name).c_str());
    constrVector_.push_back(constr);
  } else {
    cout << "ERROR: In CplexSolver::addConstr(...)" << endl;
    cout << "  Wrong comparator argument string " << comp << " supplied." << endl;
    cout << "  Should be '==', '<=' or '>='." << endl;
    assert(false);
    exit(0);
  }
  return constrVector_.back();
}

//const SolverSos & 
void CplexSolver::addSos1SolverSpecific(
  const string & name,
  const vector<SolverVar> & vars, 
  const vector<double> & weights) {

  unsigned int nVars    = vars.size();
  unsigned int nWeights = weights.size();
  assert(nVars == nWeights);

  SolverExpr linExpr;
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
  cout << "ERROR: Not supported in Cplex MilpSolver lib yet." << endl;
  assert(false);
  exit(0);
  model_->add(IloSOS1(*env_,
    IloNumVarArray(*env_, nVars, vars[2], vars[3])));
  //model_->newSos(lpName.c_str(), XPRB_S1, linExpr).setDir(XPRB_DN);
#endif // USE_SOS_VECTOR
}

void CplexSolver::update() {
  // nothing to do
}

void CplexSolver::exportModelAsLpFile(const string & fileNamePrefix) const {
  // For an LP format matrix use the following:
  IloCplex cplex(*model_);
  cplex.exportModel((fileNamePrefix + ".lp").c_str()); // may not work
  /*
  const_cast<SolverModel *>(model_)->
    exportProb(XPRB_LP, fileNamePrefix.c_str()); // adds lp extension by itself
  */
}

void CplexSolver::exportModelAsMpsFile(const string & fileNamePrefix) const {
  // For an MPS format matrix use the following:

  IloCplex cplex(*model_);
  cplex.exportModel((fileNamePrefix + ".mps").c_str()); // should work

  // Adds .mat extension by default,
  // but we avoid .mat extension 'coz Windows 
  // interprets this as Microsoft Access Table 
}

void CplexSolver::exportModelAsRewFile(const string & fileNamePrefix) const {
  // For an MPS format matrix use the following:
  
  IloCplex cplex(*model_);
  cplex.exportModel((fileNamePrefix + ".rew").c_str()); // UNTESTED  !!!
}

void CplexSolver::setMinimize() {
  sense_ = -1;
}

void CplexSolver::setMaximize() {
  sense_ = +1;
}

void CplexSolver::setStartValueOf(SolverVar & var, 
                                  double startValue) const {
  cerr << "CplexSolver::setStartValueOf(...) not implemented yet" << endl;
  assert(false);
  //var.set(GRB_DoubleAttr_Start, startValue);
}

// Everything is handled here, nothing is thrown out
bool CplexSolver::solve(double gap, int nThreads) {
  cout << "In CplexSolver::solve()" << endl;
  
  IloObjective obj;
  if (sense_ == -1) {
    obj = IloMinimize(*env_, *objFunction_);
  } else {
    assert(sense_ == +1);
    obj = IloMaximize(*env_, *objFunction_);
  }
  model_->add(obj);

  global_cplex_ = 0;
  try {
    global_cplex_ = getLicense(maxGetLicenseSeconds_); 
  } catch (string &str) { // so catch it an re-throw
    cout << str << endl;
    return false;
  }
  global_cplex_->extract(*model_);
  
  setNThreads(nThreads); // now that global_cplex_ has a propoer value (!=0)
  
#ifdef _WIN32
  string cplexLogFileName = "out\\cplex.out"; // hard coded Windows dir
#else
  string cplexLogFileName = "out/cplex.out"; // hard coded *nix dir
#endif

  const bool redirectAllOutput = false;
  if (redirectAllOutput) {
    ofstream cplexLogStr(cplexLogFileName.c_str());
	  global_cplex_->setOut(cplexLogStr);
  }
  global_cplex_->setParam(IloCplex::TiLim, maxSolveSeconds_);
  global_cplex_->setParam(IloCplex::EpGap, gap); // was 0.05

  solved_ = false;
  (void)global_cplex_->solve(/*"g"*/); 
  /* g for XPRESS meant: solve the problem as MIP */

  IloAlgorithm::Status mipStatus = global_cplex_->getStatus();
  cout << "MIP solver status = " << mipStatus << endl;

  //solved_ = false;
  if (mipStatus == IloAlgorithm::Unknown) { // 0
    cout << "WARNING: Model solution state is unknown." << endl;
    solved_ = false;
  } else if (mipStatus == IloAlgorithm::Feasible) { // 1
    cout << "WARNING: Model is feasible but [solution is not optimal?]." << endl;
    solved_ = true;
  } else if (mipStatus == IloAlgorithm::Optimal) { // 2
    cout << "The model is solved optimally." << endl;
    solved_ = true;
  } else if (mipStatus == IloAlgorithm::Infeasible) { // 3
    cout << "WARNING: Model is infeasible." << endl;
    solved_ = false;
  } else if (mipStatus == IloAlgorithm::Unbounded) { // 4
    cout << "WARNING: Model is unbounded." << endl;
    solved_ = false;
  } else if (mipStatus == IloAlgorithm::InfeasibleOrUnbounded) { // 5
    cout << "WARNING: Model is infeasible or unbounded." << endl;
    solved_ = false;
  } else if (mipStatus == IloAlgorithm::Error) { // 6
    cout << "WARNING: Model solution gave error." << endl;
    solved_ = false;
  } else {
    cout << "WARNING: Model solution unknown status." << endl;
    solved_ = false;
  }

  if (solved_) {
    cout << "NOTE: Integer solution found!" << endl;
    cout << "Objective: " << getObjVal() << endl;
  } else {
    cout << "WARNING: No integer solution found!" << endl;
  }

  return solved_;
}

double CplexSolver::getValueOf(const SolverExpr & expr) const {
  return global_cplex_->getValue(expr);
}

double CplexSolver::getValueOf(const SolverVar & var) const {
  return global_cplex_->getValue(var);
}

string CplexSolver::getName(const SolverVar & var) const {
  return var.getName();
}

double CplexSolver::getLowerBound(const SolverVar & var) const {
  return var.getLB();
}

double CplexSolver::getUpperBound(const SolverVar & var) const {
  return var.getUB();
}

void CplexSolver::setLowerBound(SolverVar & var, double value) {
	var.setLB(value);
}

void CplexSolver::setUpperBound(SolverVar & var, double value) {
	var.setUB(value);
}

double CplexSolver::getObjVal() const {
  double objValue = global_cplex_->getObjValue();
  return objValue;
}

unsigned int CplexSolver::getNumberOfRows() const {
  int rows;
  //model_->getProperties();  
  IloCplex cplex(*model_);
  rows = cplex.getNrows();
  //rows = global_cplex_->getNrows(); // global_cplex_ seems to be 0!
  return (unsigned int)rows;
}

unsigned int CplexSolver::getNumberOfColumns() const {
  int cols;
  
  IloCplex cplex(*model_);
  cols = cplex.getNcols();
  //cols = global_cplex_->getNcols();  
  assert(cols >= 0);
  return (unsigned int)cols;
}

unsigned int CplexSolver::getNumberOfNonZeroes() const {
  int nonZeroes;
  IloCplex cplex(*model_);
  nonZeroes = cplex.getNNZs();
  //nonZeroes = global_cplex_->getNNZs();
  assert(nonZeroes >= 0);  
  return (unsigned int)nonZeroes;
}

unsigned int CplexSolver::getNumberOfSets() const {
  int nSOSs;
  IloCplex cplex(*model_);
  nSOSs = cplex.getNSOSs();
  //nSOSs = global_cplex_->getNSOSs();
  assert(nSOSs >= 0);
  
  return (unsigned int)nSOSs;
}

unsigned int CplexSolver::getNumberOfSetMembers() const {
  // FIXME
  return 0;
}

void CplexSolver::calcAnIIS() const {
  cout << "ERROR: calcAnIIS: unimplemented still." << endl;
  assert(false);
}

void CplexSolver::reportAnIISTo(ostream & ostr) const {
  ostr << "ERROR: reportAnIIS: unimplemented still." << endl;
  assert(false);
}


double CplexSolver::getInfinity() const {
  return IloInfinity;
}

void CplexSolver::deleteModelAndEnv() {
  delete model_;
  model_ = 0;

  env_->end();
  delete env_;
  env_ = 0;

  delete global_cplex_;
  global_cplex_ = 0;
}

void CplexSolver::setNThreads(int nThreads) {
  cerr << "CplexSolver::setNThreads(unsigned int nThreads) not supported yet" 
  << endl;
  
  assert(global_cplex_ != 0);
  /*
  global_cplex_ = 0;
  try {
    global_cplex_ = getLicense(maxGetLicenseSeconds_); 
  } catch (string &str) { // so catch it an re-throw
    cout << str << endl;
    //return false;
    assert(false);
    exit(0);
  }
  */
  
  try {
    IloCplex cplex(*model_);
    cplex.setParam(IloCplex::Threads, (int)nThreads);
    //cplex.setParam(Threads, (int)nThreads);
    //cplex.setParam(IloCplex::SolnPoolAGap, 0); // just a test to compile things
    //cplex.setParam(CPX_PARAM_MIPTHREADS, (int)nThreads);
  } catch (IloCplex::Exception& ex) {
    cerr << "Exception catched" << endl;
    cerr << "str = " << string(ex.getMessage()) << endl;
  } catch (string &str) { // so catch it an re-throw
    cerr << str << endl;
    //throw str;
  }  //model_->getEnv().set(GRB_IntParam_Threads, nThreads); // C++
}

// throws string:
// - if license not gotten or 
// - any other unrecoverable cplex license get problem
// no other object types are thrown out
IloCplex * CplexSolver::getLicense(int maxLicenseGetSeconds) {
  IloCplex * cplex = 0;

  bool gotLicense = false;
	// Seed the random-number generator with current time so that
	// the numbers will be different every time we run.
	time_t t0=time(0); 
  srand((unsigned)time(NULL)); 
  string str;
  int errorCode;

	cout << "Trying to get Cplex license for the next " 
	  << maxLicenseGetSeconds << " seconds ..." << endl;
	while (!gotLicense) {
		double dt = difftime(time(0), t0);
		cout << dt << " seconds" << endl;
		if (dt > (double)maxLicenseGetSeconds) {
			stringstream strm; 
			strm << message[MESSAGE_NO_TOKEN] << maxLicenseGetSeconds << " sec."; 			
			throw strm.str();
    }

    try {
      cplex = new IloCplex(*env_); 
      gotLicense = true;
			cout << "Got Cplex license." << endl;
    } catch (IloCplex::Exception& ex) {
			cout << "Did not get Cplex license." << endl;
      if (ex.getStatus() == 32201) {
        //cout << ex << endl; 			
			  istringstream flux(ex.getMessage()); 
			  flux >> str >> str >> str >> str >> str >> errorCode; 
			  switch(errorCode) {
			    case 16 : // No Licence file found
						cout << "No license file found" << endl;
				    throw string(message[MESSAGE_NO_LICENSE]);
				    break; 
			    case 15 :  // No License Token 
					  {
				    //std::cout << "License Wait .... " << ++n << std::endl; 
				    unsigned int nSeconds =(rand() * 2000 / RAND_MAX); 
						cout << "No license token gotten" << endl;
						cout << "Sleeping " << nSeconds 
							<< " seconds before next license poll." << endl;
						anyOSSleep(nSeconds);
					  }
				    break; 
			    case 4 :   // No License Server
						cout << "No license server found." << endl;
				    //throw string(ex.getMessage()); 
				    throw string(message[MESSAGE_NO_SERVER]); 
				    break;
			    default:
						cout << "Unknown license problem." << endl;
				    throw string(ex.getMessage()); 
   	    }
      } else { 
        throw string(ex.getMessage()); 
      }
      // maybe new IloCplex can throw a string too      
    } catch (string &str) { // so catch it an re-throw
      throw str;
    }
	}
  return cplex; // 0 if bad
}


void CplexSolver::setBranchingMethod(int method) {
  /* Gurobi version:
  assert(-1 <= method);
  assert(method <= 3);
  model_->getEnv().set(GRB_IntParam_VarBranch, method); // C++		 
  */
  cerr << "WARNING: CplexSolver::setBranchingMethod not implemented yet" 
  << endl;
  cerr << "  So leaving all branching settings to default values." << endl;
}

////////////////// End Cuts Control ///////////////


void CplexSolver::setIntFeasTol(double value) {
  try {
    IloCplex cplex(*model_);
    cplex.setParam(IloCplex::EpInt, (int)value);
  } catch (IloCplex::Exception& ex) {
    cerr << "Exception catched" << endl;
    cerr << "str = " << string(ex.getMessage()) << endl;
    exit(0);
  } catch (string &str) {
    cerr << str << endl;
    // throw str;
    exit(0);
  }
}

CplexSolver::~CplexSolver() {
  deleteModelAndEnv();
}

#endif // USE_CPLEX_NATIVE
