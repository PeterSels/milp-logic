#include <iostream>
#include <time.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
  // for Sleep
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
			    //std::cerr << "License Wait .... " << ++n  << std::endl;
          {
			      int val = (rand() * 2000 / RAND_MAX);
			      Sleep(val);
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
    cerr << "ERROR: Cplex exception catched at calling new SolverModel()" << endl;
  }
  if (model_==0) {
    // somehow not printed
    cerr << "ERROR: Could not initialize Cplex problem." << endl;
    cerr << "  Possible cause is that another Cplex is already running," << endl;
    cerr << "  or that your license server is not correctly set up." << endl;
    cerr << "Quitting." << endl;
    assert(false);
    exit(0);
  }
  objFunction_ = new SolverExpr(*global_env_);
  // Set the objective function
  // special to Cplex compared to Gurobi:
  //////////model_->add(objFunction_); 
}

const SolverVar & CplexSolver::addLpVar(
  double lo, double hi, double objCoef, const std::string & name) {

  IloNumVar var(*env_, lo, hi, ILOFLOAT);
  var.setName(lpConvert(name).c_str());
  varVector_.push_back(var);
  // explicit store objCoef in obj function
  *objFunction_ += objCoef * var;
  return varVector_.back();
}

const SolverVar & CplexSolver::addIntVar(
  int lo, int hi, double objCoef, const std::string & name) {

  IloNumVar var(*env_, lo, hi, ILOINT);
  var.setName(lpConvert(name).c_str());
  varVector_.push_back(var);
  // explicit store objCoef in obj function
  *objFunction_ += objCoef * var;
  return varVector_.back();
}

const SolverVar & CplexSolver::addBinVar(
  double objCoef, const std::string & name) {
  IloNumVar var(*env_, 0, 1, ILOBOOL);
  var.setName(lpConvert(name).c_str());
  varVector_.push_back(var);
  // explicit store objCoef in obj function
  *objFunction_ += objCoef * var;
  return varVector_.back();
}

const SolverExpr & CplexSolver::addExpr(
  const SolverExpr & expr, const std::string & name) {
  //exprVector_.push_back(model_->addExpr(expr, name.str());
  exprVector_.push_back(expr);
  return exprVector_.back();
}

const SolverConstr & CplexSolver::addConstr(
  const SolverVar & lhs, const std::string & comp, const SolverVar & rhs, 
  const std::string & name) {
  
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
    cerr << "ERROR: In CplexSolver::addConstr(...)" << endl;
    cerr << "  Wrong comparator argument string " << comp << " supplied." << endl;
    cerr << "  Should be '==', '<=' or '>='." << endl;
    assert(false);
    exit(0);
  }
  return constrVector_.back();
}

const SolverConstr & CplexSolver::addConstr(
  const SolverExpr & lhs, const std::string & comp, const SolverVar & rhs, 
  const std::string & name) {
  
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
    cerr << "ERROR: In CplexSolver::addConstr(...)" << endl;
    cerr << "  Wrong comparator argument string " << comp << " supplied." << endl;
    cerr << "  Should be '==', '<=' or '>='." << endl;
    assert(false);
    exit(0);
  }
  return constrVector_.back();
}

const SolverConstr & CplexSolver::addConstr(
  const SolverExpr & lhs, const std::string & comp, const SolverExpr & rhs, 
  const std::string & name) {

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
    cerr << "ERROR: In CplexSolver::addConstr(...)" << endl;
    cerr << "  Wrong comparator argument string " << comp << " supplied." << endl;
    cerr << "  Should be '==', '<=' or '>='." << endl;
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
  cerr << "ERROR: Not supported in Cplex MilpSolver lib yet." << endl;
  assert(false);
  exit(0);
  model_->add(IloSOS1(*env_,
    IloNumVarArray(*env_, nVars, vars[2], vars[3])));
  //model_->newSos(lpName.c_str(), XPRB_S1, linExpr).setDir(XPRB_DN);
#endif // USE_SOS_VECTOR
}



void CplexSolver::exportModelAsLpFile(const string & fileNamePrefix) const {
  cout << "exportModelAsLpFile" << endl;
  // For an LP format matrix use the following:
  IloCplex cplex(*model_);
  cplex.exportModel((fileNamePrefix + ".lp").c_str()); // may not work
  /*
  const_cast<SolverModel *>(model_)->
    exportProb(XPRB_LP, fileNamePrefix.c_str()); // adds lp extension by itself
  */
}

void CplexSolver::exportModelAsMpsFile(const string & fileNamePrefix) const {
  cout << "exportModelAsMpsFile" << endl;
  // For an MPS format matrix use the following:

  IloCplex cplex(*model_);
  cplex.exportModel((fileNamePrefix + ".mps").c_str()); // should work

  // Adds .mat extension by default,
  // but we avoid .mat extension 'coz Windows 
  // interprets this as Microsoft Access Table 
}




void CplexSolver::setMinimize() {
  //-//model_->add(*objFunction_); // CHECKME: do this only here 
  // io at every oobjFunction_ change?
  //IloObjective obj(*env_, *objFunction_, IloObjective::Minimize);  

  //model_->add(IloMinimize(env, *objFunction_);
  sense_ = -1;
}

void CplexSolver::setMaximize() {
  //-//model_->add(*objFunction_); // CHECKME: do this only here 
  // io at every objFunction_ change?
  //IloObjective obj(*env_, *objFunction_, IloObjective::Maximize);  

  //model_->add(IloMaximize(env, objFunction_);
  sense_ = +1;
}

bool CplexSolver::solve(double gap) {
  cout << "In CplexSolver::solve()" << endl;

  /* Cplex:
  0 no messages printed;
  1 error messages only printed;
  2 warnings and errors printed;
  3 warnings, errors, and Optimizer log printed (default);
  4 all messages printed.
  */  

///////

  IloObjective obj;
  if (sense_ == -1) {
    obj = IloMinimize(*env_, *objFunction_);
  } else {
    assert(sense_ == +1);
    obj = IloMaximize(*env_, *objFunction_);
  }
  model_->add(obj); 

  global_cplex_ = 0;
  global_cplex_ = getLicense(maxGetLicenseSeconds_); 
  global_cplex_->extract(*model_);
  /*
  const bool doLpModelExport = false;
  if (doLpModelExport) {
    string fileNamePrefix = "CplexSolver";
    global_cplex_->exportModel((fileNamePrefix + ".lp").c_str());
  }
  */

/////////////////////////
  string cplexLogFileName = "out\\cplex.out"; // hard coded Windows dir
  ofstream cplexLogStr(cplexLogFileName.c_str());
	global_cplex_->setOut(cplexLogStr);
  //cplex.exportModel(getName(nameFile,"LP")); // parent will do this later

  //cout << "Go solver " << endl;
  //TUTimer myClock;
  //cplex.setParam(IloCplex::EpGap,0.05);
  //cpxMaster.setParam(IloCplex::AdvInd,0);
  //cplex.setParam(IloCplex::MIPEmphasis,2);
  global_cplex_->setParam(IloCplex::TiLim, maxSolveSeconds_);
  global_cplex_->setParam(IloCplex::EpGap, gap); // was 0.05
  //cpxMaster.setParam(IloCplex::TreLim,MaxTreeSize);

///////////////
  /*
  IloObjective obj(*env_, *objFunction_, 
    (sense == -1) ? IloObjective::Minimize : IloObjective::Maximize);  
  */
  ///////

  //IloCplex cplex(*model_);

  //model_->setMsgLevel(3);
  //cplex.setAnyProperty(  ... );
  //solver().setVerbose(Lpsolve.IMPORTANT);

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
    cout << "WARNING: No integer solution found!" << endl;
    cout << "ERROR: Model solution gave error." << endl;
    assert(!solved_);
    assert(false);
    exit(0);
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

double CplexSolver::getObjVal() const {
  double objValue = global_cplex_->getObjValue();
  return objValue;
}


unsigned int CplexSolver::getNumberOfRows() const {
  int rows;
  /*
  xo_prob_struct * opt_prob = model_->getXPRSprob();
  XPRSgetintattrib(opt_prob, XPRS_ORIGINALROWS, &rows);  
  assert(rows >= 0);
*/
  model_->getProperties();

  rows = 0;
  return (unsigned int)rows;
}

unsigned int CplexSolver::getNumberOfColumns() const {
  int cols;

  //return model_->getAnyProperty(); //GRB_IntAttr_NumVars);
  //return model_->CPXgetnumcols();

  //IloCplex cplex(*model_);
  
  //return model_->getProperty("cols");


/*
  ///model_->getIntAttrib(XPRS.ORIGINALCOLS);
  xo_prob_struct * opt_prob = model_->getXPRSprob();
  XPRSgetintattrib(opt_prob, XPRS_ORIGINALCOLS, &cols);  
  assert(cols >= 0);
  */
  cols = 0;
  return (unsigned int)cols;
}

unsigned int CplexSolver::getNumberOfNonZeroes() const {
  int nonZeroes;
  /*
  xo_prob_struct * opt_prob = model_->getXPRSprob();
  XPRSgetintattrib(opt_prob, XPRS_NZS, &nonZeroes);  
  assert(nonZeroes >= 0);
  */
  nonZeroes = 0; // FIXME
  return (unsigned int)nonZeroes;
}


unsigned int CplexSolver::getNumberOfSets() const {
  // FIXME
  return 0;
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

IloCplex * CplexSolver::getLicense(int toToken) {
  IloCplex * cplex = 0;

  bool gotLicense = false;
  int n=0;
   /* Seed the random-number generator with current time so that
    * the numbers will be different every time we run.
    */
	time_t t0=time(0); 
  srand((unsigned)time(NULL)); 
  string str;
  int errorCode, val;

	while (!gotLicense) {
		// To Token
		if (difftime(time(0), t0) > (double)toToken) {
			stringstream strm; 
			strm << message[MESSAGE_NO_TOKEN] << toToken << " sec."; 			
			throw strm.str(); 
    }
    try {
      cplex = new IloCplex(*env_); 
      //cplex.setOut(log); 
      //log << "Antoine joubert " << std::endl; 
      gotLicense=true; 	
    } catch (IloCplex::Exception& ex) {
      if (ex.getStatus() == 32201) {
        //cerr << ex << endl; 			
			  istringstream flux(ex.getMessage()); 
			  flux >> str >> str >> str >> str >> str >> errorCode; 
			  switch(errorCode) {
			    case 16 : // LIcence file not found
				    throw string(message[MESSAGE_NO_LICENSE]); 
				    break; 
			    case 15 :  // No Token 
				    //std::cerr << "License Wait .... " << ++n << std::endl; 
				    val=(rand()*2000/RAND_MAX); 
				    Sleep(val); 
				    break; 
			    case 4 :   // No Server
				    //throw string(ex.getMessage()); 
				    throw string(message[MESSAGE_NO_SERVER]); 
				    break; 
			    default:
				    throw string(ex.getMessage()); 
   	    }	
      } else {
        throw string(ex.getMessage()); 
      }
    } 
    catch (string &str) {
      throw str; 
    }
	}
  return cplex;
}


CplexSolver::~CplexSolver() {
  deleteModelAndEnv();
}

#endif // USE_CPLEX_NATIVE
