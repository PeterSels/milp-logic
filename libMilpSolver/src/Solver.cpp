#include <sstream>
#include <assert.h>

#include "Solver.h"
#include "StringUtilities.h"
// for replaceAllOddByEven

#define MANUALLY_ADD_SOS_CONSTRAINTS (true)
#define ADD_SOLVER_SOS (false)

#ifdef USE_CPLEX_NATIVE
extern IloEnv * global_env_;
#endif

using namespace std;

Solver::Solver()
:  model_(0)
,  solved_(false)
{
  varVector_.clear();
  exprVector_.clear();
  constrVector_.clear();
}

const SolverVar Solver::addConjunctionBinVar(
  double objCoef,
  const SolverVar & conjunctionBinVarB, 
  const SolverVar & conjunctionBinVarC,
  const std::string & name) {
  
  const SolverVar conjunctionBinVarA = addBinVar(objCoef, name);
  addConjunctionConstr(
    conjunctionBinVarA, conjunctionBinVarB, conjunctionBinVarC, name);
  return conjunctionBinVarA;
}

void Solver::addConjunctionConstr(
  const SolverVar & conjunctionBinVarA, 
  const SolverVar & conjunctionBinVarB, 
  const SolverVar & conjunctionBinVarC,
  const std::string & name) {
  
  addConstr(conjunctionBinVarA, "<=", conjunctionBinVarB, name + "_a_le_b");
  addConstr(conjunctionBinVarA, "<=", conjunctionBinVarC, name + "_a_le_c");
  SolverExpr oneExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
	oneExpr += 1;
	
	
	SolverExpr expr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
	expr += conjunctionBinVarB;
	expr += conjunctionBinVarC;
  addConstr(expr, "<=", 
            SolverExpr(conjunctionBinVarA+oneExpr), name + "_b_plus_c_le_1_plus_a");  // - -> + !!!
}

const SolverVar Solver::addDisjunctionBinVar(
  double objCoef,
  const SolverVar & disjunctionBinVarB, 
  const SolverVar & disjunctionBinVarC,
  const std::string & name) {
  
  const SolverVar disjunctionBinVarA = addBinVar(objCoef, name);  
  addDisjunctionConstr(
    disjunctionBinVarA, disjunctionBinVarB, disjunctionBinVarC, name);
  return disjunctionBinVarA;
}

void Solver::addDisjunctionConstr(
  const SolverVar & disjunctionBinVarA, 
  const SolverVar & disjunctionBinVarB, 
  const SolverVar & disjunctionBinVarC,
  const std::string & name) {
  
  addConstr(disjunctionBinVarA, ">=", disjunctionBinVarB, name + "_a_ge_b");
  addConstr(disjunctionBinVarA, ">=", disjunctionBinVarC, name + "_a_ge_c");
	
	SolverExpr expr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
	expr += disjunctionBinVarB;
	expr += disjunctionBinVarC;
	
  addConstr(SolverExpr(disjunctionBinVarA), "<=", 
            expr, name + "_a_le_b_plus_c");
}

void Solver::addNegationConstr(
  const SolverVar & binVarA, 
  const SolverVar & binVarB, 
  const std::string & name) {

  SolverExpr oneExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
	oneExpr += 1;	
  addConstr(binVarA, "==", oneExpr - binVarB, name + "_a_not_b");
}


const SolverVar Solver::addEqualBinVar(
  double objCoef,
  const SolverExpr & lhsExpr,
  double lhsLowerBound,  double lhsUpperBound,
  const SolverExpr & rhsExpr,
  double rhsLowerBound,  double rhsUpperBound,
  double unit,
  const std::string & name) {

  const SolverVar eqBinVar = addBinVar(objCoef, name);
  addEqualConstr(eqBinVar,
								 lhsExpr, lhsLowerBound, lhsUpperBound, 
								 rhsExpr, rhsLowerBound, rhsUpperBound,
								 unit,
								 name);
  return eqBinVar;
}

void Solver::addEqualConstr(
  const SolverVar & eqBinVar,
  const SolverExpr & lhsExpr,
  double lhsLowerBound,  double lhsUpperBound,
  const SolverExpr & rhsExpr,
  double rhsLowerBound,  double rhsUpperBound,
  double unit,
  const std::string & name) {

	/* If defining a binVar p as equivalent to both a<=b as well as to a>=b this variable cannot be false.
	 Since if false, then a>b *and* (io or) a<b. So p is true.
	// == when <= and >=
	addLessOrEqualConstr(eqBinVar, 
											 lhsExpr, lhsLowerBound, lhsUpperBound,  // lhs
											 rhsExpr, rhsLowerBound, rhsUpperBound,  // rhs non-swapped
											 unit, 
											 name + "_eq_le");
	addLessOrEqualConstr(eqBinVar, 
											 rhsExpr, rhsLowerBound, rhsUpperBound,  // rhs
											 lhsExpr, lhsLowerBound, lhsUpperBound,  // lhs swapped!
											 unit, 
											 name + "_eq_ge");
  */
	
	// defining two binVars: le and ge and letting eq = conjunction(le, ge) would work but gives lots of equations
	
	/*
	// shorter is:
	// When eqBinVar==1, this constraints lhs == rhs, just fine
	// but when eqBinVar==0 this does not constrain anything 
  double Mle1 = (lhsUpperBound - rhsLowerBound);
  addConstr(lhsExpr - rhsExpr + (Mle1 * eqBinVar), 
						"<=", Mle1, name + "_eq_1");
  double Mle0 = (lhsLowerBound - rhsUpperBound);
	addConstr(lhsExpr - rhsExpr + (Mle0 * eqBinVar), 
						">=", Mle0, name + "_eq_0");
	
	 */

	 // == when <= and >=
	 SolverVar leBinVar =
	 addLessOrEqualBinVar(0,
	   lhsExpr, lhsLowerBound, lhsUpperBound,  // lhs
	   rhsExpr, rhsLowerBound, rhsUpperBound,  // rhs non-swapped
	   unit, 
	   name + "_eq_le");
 	 SolverVar geBinVar =
	 addLessOrEqualBinVar(0, 
	   rhsExpr, rhsLowerBound, rhsUpperBound,  // rhs
	   lhsExpr, lhsLowerBound, lhsUpperBound,  // lhs swapped!
	   unit, 
	   name + "_eq_ge");
	 addConjunctionConstr(eqBinVar, leBinVar, geBinVar, name + "_eq_le" + "_eq_ge" + "_conj");	
}

const SolverVar Solver::addEqualBinVar(
  double objCoef,
  const SolverExpr & lhsExpr,
  double lhsLowerBound,  double lhsUpperBound,
  double rhs,
  double unit,
  const std::string & name) {

  const SolverVar eqBinVar = addBinVar(objCoef, name);
  addEqualConstr(eqBinVar,
								 lhsExpr-rhs, lhsLowerBound-rhs, lhsUpperBound-rhs, 
								 0,
								 unit,
								 name);
  return eqBinVar;
}

void Solver::addEqualConstr(
  const SolverVar & eqBinVar,
  const SolverExpr & lhsExpr,
  double lhsLowerBound,  double lhsUpperBound,
  double rhs,
  double unit,
  const std::string & name) {

	 // == when <= and >=
	 SolverVar leBinVar =
	 addLessOrEqualBinVar(0,
	   lhsExpr-rhs, lhsLowerBound-rhs, lhsUpperBound-rhs, // lhs // rhs non-swapped
     0,
	   unit, 
	   name + "_eq_le");
 	 SolverVar geBinVar =
	 addLessOrEqualBinVar(0, 
	   rhs-lhsExpr, rhs-lhsLowerBound, rhs-lhsUpperBound, // rhs // lhs swapped!
     0,
	   unit, 
	   name + "_eq_ge");
	 addConjunctionConstr(eqBinVar, leBinVar, geBinVar, 
     name + "_eq_le" + "_eq_ge" + "_conj");	
}

const SolverVar Solver::addLessOrEqualBinVar(double objCoef,
																						 const SolverExpr & lhsExpr,
																						 double lhsLowerBound,  double lhsUpperBound,
																						 const SolverExpr & rhsExpr,
																						 double rhsLowerBound,  double rhsUpperBound,
																						 double unit,
																						 const std::string & name) {
	
  const SolverVar leBinVar = addBinVar(objCoef, name);
  addLessOrEqualConstr(leBinVar,
                       lhsExpr, lhsLowerBound, lhsUpperBound, 
                       rhsExpr, rhsLowerBound, rhsUpperBound,
											 unit,
                       name);
  return leBinVar;
}

void Solver::addLessOrEqualConstr(const SolverVar & leBinVar,
																	const SolverExpr & lhsExpr,
																	double lhsLowerBound,  double lhsUpperBound,
																	const SolverExpr & rhsExpr,
																	double rhsLowerBound,  double rhsUpperBound,
																	double unit,
																	const std::string & name) {
	
	// real constraint when leBinVar=1, says nothing new when 0
  double Mle1 = (lhsUpperBound - rhsLowerBound);
  SolverExpr rhs
#ifdef USE_CPLEX_NATIVE
    (*global_env_)
#endif
   ;
  rhs+= Mle1;
  addConstr(lhsExpr - rhsExpr + (Mle1 * leBinVar), 
						"<=", rhs, name + "_le_1");

	// real constraint when leBinVar=0, says nothing new when 1
  double Mle0 = (lhsLowerBound - rhsUpperBound);  
  addConstr(lhsExpr - rhsExpr, 
						">=", (Mle0 - unit) * leBinVar + unit, name + "_le_0"); //  not('lhs < rhs') is equivalent to 'lhs >= rhs + unit'
}

const SolverVar Solver::addLessOrEqualBinVar(double objCoef,
																						 const SolverExpr & lhsExpr,
																						 double lhsLowerBound,  double lhsUpperBound,
																						 double rhs,
																						 double unit,
																						 const std::string & name) {
	
  const SolverVar leBinVar = addBinVar(objCoef, name);
  addLessOrEqualConstr(leBinVar,
                       lhsExpr, lhsLowerBound, lhsUpperBound, 
                       rhs,
											 unit,
                       name);
  return leBinVar;
}

void Solver::addLessOrEqualConstr(const SolverVar & leBinVar,
																	const SolverExpr & lhsExpr,
																	double lhsLowerBound,  double lhsUpperBound,
																	double rhs, 
																	double unit,
																	const std::string & name) {
	
	// real constraint when leBinVar=1, says nothing new when 0
  double Mle1 = (lhsUpperBound - rhs);
  SolverExpr newRhs
#ifdef USE_CPLEX_NATIVE
    (*global_env_)
#endif
   ;
  newRhs+= Mle1;
  addConstr(lhsExpr - rhs + (Mle1 * leBinVar), 
						"<=", newRhs, name + "_le_1");

	// real constraint when leBinVar=0, says nothing new when 1
  double Mle0 = (lhsLowerBound - rhs);  
  addConstr(lhsExpr - rhs, 
						">=", (Mle0 - unit) * leBinVar + unit, name + "_le_0"); 
  //  not('lhs < rhs') is equivalent to 'lhs >= rhs + unit'
}

void Solver::addImplication(
  const SolverVar & binVarA, 
  const SolverVar & binVarB, 
  const std::string & name) {
  
  addConstr(binVarA, "<=", binVarB, name + "_a_implies_b");
}

void Solver::addEquivalence(
  const SolverVar & binVarA, 
  const SolverVar & binVarB, 
  const std::string & name) {
  
  addConstr(binVarA, "<=", binVarB, name + "_equiv_a_implies_b");
  addConstr(binVarB, "<=", binVarA, name + "_equiv_b_implies_a");
}

void Solver::addSos1(const SolverVar & x) {
  double loDouble = (int)getLowerBound(x);
  double hiDouble = (int)getUpperBound(x);
  int lo = (int)loDouble;
  int hi = (int)hiDouble;
  assert(lo == loDouble);
  assert(hi == hiDouble);

  string xName = getName(x);
  
  assert( 0 <= lo);
  assert( 0 <= hi);
  assert(lo <= hi);
	
	SolverExpr xSos1SumExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;	

	SolverExpr xSos1ScalarProductExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;	

	SolverExpr oneExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;	
	oneExpr += 1;
	
  vector<SolverVar> sosVarVector;
  vector<double> sosWeightVector;
  for (unsigned int i=(unsigned int)lo; i<=(unsigned int)hi; i++) {
    double objCoef = 0;
    stringstream strstr;
    strstr << xName << "_bin_" << i;
    const SolverVar iBinVar = addLpVar(0, 1, (int)objCoef, strstr.str());
    sosVarVector.push_back(iBinVar);
    sosWeightVector.push_back(i);
    // We suppose that the SolverVar x has been added with the correct coefficient
    // for the goal function. So there is no need to add a goal function coefficient
    // anymore for any of the iBinVars.
    
    xSos1SumExpr             += iBinVar;
    if (i!=0) {
      xSos1ScalarProductExpr += (int)i * iBinVar;
    }
  }
  bool manuallyAddConstraints = MANUALLY_ADD_SOS_CONSTRAINTS;
  if (manuallyAddConstraints) {
    // according to http://lpsolve.sourceforge.net/5.0/SOS.htm
    addConstr(xSos1SumExpr,             "==", oneExpr, 
      xName + "_sos1_sum_is_1"); // convexity row (3)
    addConstr(xSos1ScalarProductExpr, "==", x,             
      xName + "_sos1_scalar_prod_is_x"); // reference row (2)
  }
  bool addSolverSos = ADD_SOLVER_SOS;
  if (addSolverSos) {
    stringstream strstr;
    strstr << xName << "_solver_sos_" << lo << "_" << hi;
    string xSosName = strstr.str();
    addSos1SolverSpecific(xSosName, sosVarVector, sosWeightVector); 
      // makes some of the 3 constraints above redundant, 
      // which one(s)?...FIXME
  }
}

void Solver::addSos1(const SolverVar & x, 
										 const SolverVar & z, 
										 double (*fPtr)(int)) {
  
  double loDouble = getLowerBound(x);
  int lo = (int)loDouble;
  assert(lo == loDouble);
	
  double hiDouble = getUpperBound(x);
  int hi = (int)hiDouble;
  assert(hi == hiDouble);
  string xName = getName(x);
	
  assert( 0 <= lo);
  assert( 0 <= hi);
  assert(lo <= hi);
	
	SolverExpr xSos1SumExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;  
	
	SolverExpr xSos1ScalarProductExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	; 	
	
	SolverExpr zSos2ScalarProductExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	; 	
	
  vector<SolverVar> sosVarVector;
  vector<double> sosWeightVector;
  for (unsigned int i=(unsigned int)lo; i<=(unsigned int)hi; i++) {
    double objCoef = 0;
    stringstream strstr;
    strstr << xName << "_bin_" << i;
    //const SolverVar iBinVar = addLpVar(0, 1, (int)objCoef, strstr.str());
    const SolverVar iBinVar = addBinVar((int)objCoef, strstr.str());
    sosVarVector.push_back(iBinVar);
    sosWeightVector.push_back(i);
    // We suppose that either the SolverVar x or
		// its corresponding cost SolverVar y 
		// has been added with the correct coefficient
    // for the goal function. 
		// So there is no need to add a goal function coefficient
    // anymore for any of the iBinVars.
    
    xSos1SumExpr           += iBinVar;
    xSos1ScalarProductExpr += iBinVar * (int)i;
    double coef = (*fPtr)(i);
    zSos2ScalarProductExpr += iBinVar * coef;
  }
  bool manuallyAddConstraints = MANUALLY_ADD_SOS_CONSTRAINTS;
  if (manuallyAddConstraints) {
    // according to http://lpsolve.sourceforge.net/5.0/SOS.htm
		SolverExpr oneExpr
#ifdef USE_CPLEX_NATIVE
		(*global_env_)
#endif
		;     
		oneExpr += 1;
    addConstr(xSos1SumExpr,             "==", oneExpr, 
              xName + "_sos1_sum_is_1"); // convexity row (3)
    addConstr(xSos1ScalarProductExpr, "==", x,             
              xName + "_sos1_scalar_prod_is_x"); // reference row (2)
    addConstr(zSos2ScalarProductExpr, "==", z,             
              xName + "_sos1_scalar_prod_is_z"); // function row (1)
  }
  bool addSolverSos = ADD_SOLVER_SOS;
  if (addSolverSos) {
    stringstream strstr;
    strstr << xName << "_sososos_" << lo << "_" << hi;
    string xSosName = strstr.str();
    addSos1SolverSpecific(xSosName, sosVarVector, sosWeightVector); 
		// makes some of the 3 constraints above redundant, 
		// which one(s)?...FIXME
  }
} // addSos1

void Solver::addSumSos1(const SolverVar & x, const SolverVar & y, 
												const SolverVar & z, 
												double (*fPtr)(const vector<double> & parameters),
												vector<double> & parameters) {
	// x
	double xLoDouble = getLowerBound(x);
	int xLo = (int)xLoDouble;
	assert(xLo == xLoDouble);
	
	double xHiDouble = getUpperBound(x);
	int xHi = (int)xHiDouble;
	assert(xHi == xHiDouble);

	cout << "      xLoDouble = " << xLoDouble << endl;
	cout << "      xHiDouble = " << xHiDouble << endl;

	if (xLoDouble < 0) {
		cout << "xLoDouble = " << xLoDouble << endl;
	}
	if (xLoDouble > 30) {
		cout << "xLoDouble = " << xLoDouble << endl;
	}
	if (xHiDouble < 0) {
		cout << "xHiDouble = " << xHiDouble << endl;
	}
	if (xHiDouble > 30) {
		cout << "xHiDouble = " << xHiDouble << endl;
	}
	
	string xName = getName(x);
	
	assert( 0 <= xLo);
	assert( 0 <= xHi);
	assert(xLo <= xHi);
	
	// y
	double yLoDouble = getLowerBound(y);
	int yLo = (int)yLoDouble;
	assert(yLo == yLoDouble);
	
	double yHiDouble = getUpperBound(y);
	int yHi = (int)yHiDouble;
	assert(yHi == yHiDouble);
	
	string yName = getName(y);
	
	assert( 0 <= yLo);
	assert( 0 <= yHi);
	assert(yLo <= yHi);
	
	SolverExpr xySumSos1SumExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;  

	SolverExpr xySumSos1ScalarProductExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
		
	SolverExpr zSos2ScalarProductExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	; 	
	
	{ // xySum = x + y
		//vector<SolverVar> sosVarVector;
		//vector<double> sosWeightVector;
		
		//double xySymDummy = 0.0;
		//parameters.push_back(xySymDummy);
		const unsigned int varParamIndex = 4; // D0 happens to be at index 4
		
		for (unsigned int i=(unsigned int)xLo + yLo; 
				 i<=(unsigned int)xHi + yHi; i++) {
			double objCoef = 0;
			stringstream strstr;
			strstr << xName << "_bin_" << i;
			const SolverVar iBinVar = addBinVar((int)objCoef, strstr.str());
			//sosVarVector.push_back(iBinVar);
			//sosWeightVector.push_back(i);
			
			xySumSos1SumExpr           += iBinVar;
			xySumSos1ScalarProductExpr += iBinVar * (int)i;
			
			parameters[varParamIndex] = i;
			double coef = (*fPtr)(parameters);
			//cout << "cost(" << i << ") =" << coef << endl; // to see if we have Nans! FIXME
			
			zSos2ScalarProductExpr     += iBinVar * coef;
		}
		//parameters.pop_back();
	}
  bool manuallyAddConstraints = MANUALLY_ADD_SOS_CONSTRAINTS;
  if (manuallyAddConstraints) {
    // according to http://lpsolve.sourceforge.net/5.0/SOS.htm
		SolverExpr oneExpr
#ifdef USE_CPLEX_NATIVE
		(*global_env_)
#endif
		;     
		oneExpr += 1;
		
		SolverExpr xySumExpr
#ifdef USE_CPLEX_NATIVE
		(*global_env_)
#endif
		;     
		xySumExpr += x;
		xySumExpr += y;
		
    addConstr(xySumSos1SumExpr,           "==", oneExpr, 
              xName + "_sos1_sum_is_1"); // convexity row (3)
    addConstr(xySumSos1ScalarProductExpr, "==", xySumExpr,             
              xName + "_sos1_scalar_prod_is_x"); // reference row (2)
    addConstr(zSos2ScalarProductExpr,     "==", z,             
              xName + "_sos1_scalar_prod_is_z"); // function row (1)
  }
	/*
  bool addSolverSos = ADD_SOLVER_SOS;
  if (addSolverSos) {
		{
			stringstream strstr;
			strstr << xName << "_sososos_" << xLo << "_" << xHi;
			string xSosName = strstr.str();
			addSos1SolverSpecific(xSosName, sosVarVector, sosWeightVector); 
      // makes some of the 3 constraints above redundant, 
      // which one(s)?...FIXME
		}
		{
			stringstream strstr;
			strstr << yName << "_sososos_" << yLo << "_" << yHi;
			string ySosName = strstr.str();
			addSos1SolverSpecific(ySosName, sosVarVector, sosWeightVector); 
		}
  }
	*/
} // addSos1_2D



// For the lp file the solver will write out.
// from: http://lpsolve.sourceforge.net/5.5/lp-format.htm
//   A var must start with a letter (either upper or lower case), and may
//   contain any number of additional letters, numerals, or characters
//   from this list: _[]{}/.&#$%~'@^
//   The '-' character is also removed (replaced by '_') because
//   fabrice.buscaylet@artelys.com from Xpress/FICO told me so.
string Solver::lpConvert(const string & name) {
  string convName = name;
  string oddEvenStr = " _:_-_/_._";
  ::replaceAllOddByEven(convName, oddEvenStr);
  return convName;
}

Solver::~Solver() {
  cout << "in ~Solver()" << endl;  
}
