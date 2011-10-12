#include <sstream>
#include <assert.h>

#include "Solver.h"
#include "StringUtilities.h"
  // for replaceAllOddByEven
#include "MinimumCalculator.h"

//#define MANUALLY_ADD_SOS_CONSTRAINTS (true)
//#define ADD_SOLVER_SOS (false)

#ifdef USE_CPLEX_NATIVE
extern IloEnv * global_env_;
#endif

using namespace std;

Solver::Solver()
: model_(0)
, solved_(false)
, nullExpr_(0)
, oneExpr_(0)
, minusOneExpr_(0)
{
  varVector_.clear();
  exprVector_.clear();
  constrVector_.clear();
}

void Solver::resetModelNullOneExpressions() {
	// delete if exists
	if (nullExpr_!=0) {
		delete nullExpr_;
		nullExpr_ = 0;
	}
	if (oneExpr_!=0) {
		delete oneExpr_;
		oneExpr_ = 0;
	}
	if (minusOneExpr_!=0) {
		delete minusOneExpr_;
		minusOneExpr_ = 0;
	}
	
	// recreate
	nullExpr_ = new SolverExpr(
#ifdef USE_CPLEX_NATIVE
														 *global_env_
#endif
														 );
	// nullExpr_ : has correct value already
	
	oneExpr_ = new SolverExpr(
#ifdef USE_CPLEX_NATIVE
														*global_env_
#endif
														);
	*oneExpr_ += 1;	

	minusOneExpr_ = new SolverExpr(
#ifdef USE_CPLEX_NATIVE
														*global_env_
#endif
														);
	*minusOneExpr_ -= 1;	
	
	update();
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

const SolverVar Solver::
addLessOrEqualBinVar(double objCoef,
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

const SolverVar Solver::
addLessOrEqualBinVar(double objCoef,
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

void Solver::
addLessOrEqualConstr(const SolverVar & leBinVar,
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

// only used in milpSolverTest
 void Solver::addSos1(const SolverVar & x,
											bool doUpdate) {
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
  const bool manuallyAddConstraints = true; //MANUALLY_ADD_SOS_CONSTRAINTS;
  if (manuallyAddConstraints) {
    // according to http://lpsolve.sourceforge.net/5.0/SOS.htm
    addConstr(xSos1SumExpr, "==", oneExpr, 
      xName + "_sos1_sum_is_1",
							doUpdate); // convexity row (3)
    addConstr(xSos1ScalarProductExpr, "==", x,             
      xName + "_sos1_scalar_prod_is_x",
							doUpdate); // reference row (2)
  }
  
  //bool addSolverSos = ADD_SOLVER_SOS;
  //if (addSolverSos) {
  //  stringstream strstr;
  //  strstr << xName << "_solver_sos_" << lo << "_" << hi;
  //  string xSosName = strstr.str();
  //  addSos1SolverSpecific(xSosName, sosVarVector, sosWeightVector); 
  //    // makes some of the 3 constraints above redundant, 
  //    // which one(s)?...FIXME
  //}
  
}

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

BinVarMap &
Solver::addBinVarsFor(const SolverVar * x, 
											unsigned int step, 
											bool doUpdate) {
	setStep(x, step);
	
  int xLo, xHi;
  string xName = getNameLoHi(xLo, xHi, x);
	
	assert( 0 <= xLo);
	assert( 0 <= xHi);
	assert(xLo <= xHi);
			
	for (int i=xLo; i<=xHi; i+=(int)step) {		
		double objCoef = 0;
		stringstream strstr;
		strstr << xName << "_bin_" << i;
		const SolverVar iBinVar = addBinVar((int)objCoef, strstr.str(), false);
		binVarMap_[x][i] = iBinVar;
	}
	
	if (doUpdate) {
		update();
	}	

	return binVarMap_;	
}

const SolverVar & Solver::getBinVar(const SolverVar * x, 
																		int i) const {
	assert(binVarMap_.count(x)==1);
  assert(binVarMap_.find(x)->second.count(i)==1);
	return binVarMap_.find(x)->second.find(i)->second;
}

BinVarSumMap & 
Solver::addBinSumVarsFor(const SolverVar * x, 
												 const SolverVar * y, 
												 unsigned int step, 
												 bool doUpdate) {
	assert(step == getStep(x));
	assert(step == getStep(y));
	
	// x
  int xLo, xHi;
  string xName = getNameLoHi(xLo, xHi, x);
	
	// y
  int yLo, yHi;
  string yName = getNameLoHi(yLo, yHi, y);
		
	for (int i=xLo + yLo; i<=xHi + yHi; i+=(int)step) {		
		double objCoef = 0;
		stringstream strstr;
		strstr << xName << "_plus_" << yName << "_bin_" << i;
		const SolverVar iBinVar = addBinVar((int)objCoef, strstr.str(), false);
		binVarSumMap_[x][y][i] = iBinVar;
	}
	
	if (doUpdate) {
		update();
	}	
	return binVarSumMap_;
}

const SolverVar & Solver::getBinSumVar(const SolverVar * x, 
																			 const SolverVar * y, 
																			 int i) const {
	assert(binVarSumMap_.count(x)==1);
  assert(binVarSumMap_.find(x)->second.count(y)==1);
  assert(binVarSumMap_.find(x)->second.find(y)->second.count(i)==1);
	return binVarSumMap_.find(x)->second.find(y)->second.find(i)->second;
}

void Solver::addBinConvexityAndReferenceRowsFor(const SolverVar * x, 
																								bool doUpdate) {
	// according to http://lpsolve.sourceforge.net/5.0/SOS.htm
	SolverExpr convexityExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;  
	SolverExpr referenceExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
	
	unsigned int step = getStep(x);
	
	int xLo, xHi;
  string xName = getNameLoHi(xLo, xHi, x);
	
	for (int i=xLo; i<=xHi; i+=(int)step) {
		stringstream strstr;
		
		const SolverVar & iBinVar = getBinVar(x, i);
		convexityExpr += iBinVar;
		referenceExpr += iBinVar * (int)i;	
	}
	addConstr(convexityExpr, "==", *oneExpr_, 
						xName + "_bin_convexity_1",
						doUpdate); // convexity row (3)
	addConstr(referenceExpr, "==", *x,             
						xName + "_bin_reference_1",
						doUpdate); // reference row (2)	
}

void Solver::addBinConvexityAndReferenceRowsForSum(const SolverVar * x,
																								   const SolverVar * y,
																									 bool doUpdate) {
	// according to http://lpsolve.sourceforge.net/5.0/SOS.htm
	SolverExpr convexityExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;  
	SolverExpr referenceExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
	
  unsigned int step = getStep(x);
	assert(step==getStep(y));
	
	// x
	int xLo, xHi;
	string xName = getNameLoHi(xLo, xHi, x);
	
	// y
	int yLo, yHi;
	string yName = getNameLoHi(yLo, yHi, y);
	
	// x + y
	for (int i=xLo+yLo; i<=xHi+yHi; i+=(int)step) {
		const SolverVar & iBinSumVar = getBinSumVar(x, y, i);
		convexityExpr += iBinSumVar;
		referenceExpr += iBinSumVar * (int)i;	
	}
	string xPlusYName = xName + "_plus_" + yName;
	addConstr(convexityExpr, "==", *oneExpr_, 
						xPlusYName + "_bin_convexity_2",
						doUpdate); // convexity row (3)
	addConstr(referenceExpr, "==", (*x) + (*y),             
						xPlusYName + "_bin_reference_2",
						doUpdate); // reference row (2)	
}


void Solver::addSos1(const SolverVar & x, 
										 const SolverVar & z, 
										 double (*fPtr)(const vector<double> & parameters, 
																		int ii),
										 const vector<double> & parameters,
										 bool doUpdate) {
	// according to http://lpsolve.sourceforge.net/5.0/SOS.htm
	// function row (1)	
																										
	SolverExpr functionExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
	
	// x
	int xLo, xHi;
	string xName = getNameLoHi(xLo, xHi, &x);	
  unsigned int step = getStep(&x);
	
	for (int i=xLo; i<=xHi; i+=(int)step) {
		const SolverVar & iBinVar = getBinVar(&x, i);
		double coef = (*fPtr)(parameters, i);
		
		functionExpr += iBinVar * coef;
	}
	addConstr(functionExpr, "==", z,
						xName + "_bin_function_1",
						doUpdate); // function row (1)	
}

void Solver::addConvexMax(const SolverVar & x,
													const SolverVar & z,
													double (*fPtr)(const vector<double> & parameters, 
																				 int ii),
													const vector<double> & parameters,
                          const PwlApproximator & convexApprox,
													bool robust,
													bool doUpdate) {
	  
	const double D1 = parameters[1];	 // [1] is not transparant!
  
	// x
  int xLo, xHi;
	string xName = getNameLoHi(xLo, xHi, &x);
	
	if (!robust) {
		addConstr(x, "==", z, // "<=" gives same results since z is minimized
							// but == seems faster: eg: 40s io 45s
							xName + "_convex_max_up_tight_1",
							doUpdate); // function row (1)
    assert(false);
    cerr << "ERROR: not supported for now" << endl; // FIXME
		return;
	}
	
	// point 0
	double z0 = convexApprox.getZ(0); //(*fPtr)(parameters, 0);
	
	// point 1
	//double dMin = (*fPtr)(parameters, -1);
  
  //MinimumCalculator minCalc(fPtr, parameters, D1);
  //double dMin = minCalc.getMinimumAbsis();
	//double zdMin = minCalc.getMinimumValue(); //(*fPtr)(parameters, dMin);
  
  double dMin  = convexApprox.getD(1);
  double zdMin = convexApprox.getZ(1);
  
	// point 2
	double zD1 = convexApprox.getZ(2); //(*fPtr)(parameters, D1);
	
	// on the d axis:
  assert(0 <= dMin);
  assert(dMin <= D1);
	
	// on the cost axis:
  assert(zdMin <= z0);
  //assert(zdMin <= zD1); // not true anymore since used BreakPointCalulator
  // io MinimumCalculator
		
	// now imagine a V shape described by these 3 points.
		
  if (dMin <0) {
    cerr << "dMin = " << dMin << endl;
	  assert(dMin > 0);
  }
	SolverExpr dnFunctionExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
  
	if (D1 > dMin) {
		if (dMin > 0) { // only dnFunction when something left of dMin
			dnFunctionExpr += z0 + (zdMin - z0)/(dMin /*- 0*/) * (x /*- 0*/);
			addConstr(dnFunctionExpr, "<=", z,
								xName + "_convex_max_robust_dn_function",
								doUpdate); // function row (1)	
		}
		
		SolverExpr upFunctionExpr
#ifdef USE_CPLEX_NATIVE
		(*global_env_)
#endif
		;
    upFunctionExpr += 
		  zdMin + (zD1 - zdMin)/(D1 - dMin) * (x - dMin);
		addConstr(upFunctionExpr, "<=", z,
							xName + "_convex_max_robust_up_function",
							doUpdate); // function row (1)	
	} else {
		//cerr << D1 << " = D1 <= dMin = " << dMin << endl;
		assert(D1==dMin);
		assert(D1!=0);
		assert(zD1 <= z0); // dn slope
		dnFunctionExpr += 
		z0 + (zD1 - z0)/(D1 /*- 0*/) * (x /*- 0*/);
		addConstr(dnFunctionExpr, "<=", z,
							xName + "_convex_max_robust_dn_only_function",
							doUpdate);		
	}
}


void Solver::addSumSos1(const SolverVar & x, const SolverVar & y, 
												const SolverVar & z, 
												double (*fPtr)(const vector<double> & parameters, 
																			 int ii),
												const vector<double> & parameters,
												bool doUpdate) {
	// according to http://lpsolve.sourceforge.net/5.0/SOS.htm
	// function row (1)
	
	SolverExpr functionExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
	
	// x
  int xLo, xHi;
	string xName = getNameLoHi(xLo, xHi, &x);	
	// y
	int yLo, yHi;
	string yName = getNameLoHi(yLo, yHi, &y);
	// x + y
	unsigned int step = getStep(&x);
	assert(step==getStep(&y));
	
	for (int i=xLo+yLo; i<=xHi+yHi; i+=(int)step) {
		const SolverVar & iBinSumVar = getBinSumVar(&x, &y, i);		
		double coef = (*fPtr)(parameters, i);
		
		functionExpr += iBinSumVar * coef;
	}
	string xPlusYName = xName + "_plus_" + yName;
	addConstr(functionExpr, "==", z,             
						xPlusYName + "_bin_function_2",
						doUpdate); // function row (1)	
}


void Solver::addSumConvexMax(const SolverVar & x, const SolverVar & y, 
														 const SolverVar & z, 
														 double (*fPtr)(const vector<double> & parameters, 
																						int ii),
														 const vector<double> & parameters,
                             const PwlApproximator & convexApprox,
														 bool robust,
														 bool doUpdate) {
	

  const double D1 = parameters[2];	 // [2] is not transparant!

  // x
  int xLo, xHi;
	string xName = getNameLoHi(xLo, xHi, &x);	
	// y
	int yLo, yHi;
	string yName = getNameLoHi(yLo, yHi, &y);
	
	SolverExpr xPlusYExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
  xPlusYExpr += x;
  xPlusYExpr += y;
	string xPlusYName = xName + "_plus_" + yName;
	
	if (!robust) {
		addConstr(xPlusYExpr, "==", z, // "<=" gives same results since 
							// z is minimized, but == seems faster: eg: 40s io 45s
							xPlusYName + "_sum_convex_max_up_tight_1",
							doUpdate); // function row (1)
    assert(false);
    cerr << "ERROR: not supported for now" << endl; // FIXME
		return;
	}

	// point 0
	double z0 = convexApprox.getZ(0); //(*fPtr)(parameters, 0);
	
	// point 1
  
	//double dMin = (*fPtr)(parameters, -1);
  //MinimumCalculator minCalc(fPtr, parameters, D1);
  //double dMin = minCalc.getMinimumAbsis();
	//double zdMin = minCalc.getMinimumValue(); //(*fPtr)(parameters, dMin);	
  double dMin  = convexApprox.getD(1);
  double zdMin = convexApprox.getZ(1);
  
	// point 2
	double zD1 = convexApprox.getZ(2);//(*fPtr)(parameters, D1);

	// on the d axis:
  assert(0 <= dMin);
  assert(dMin <= D1);

	// on the cost axis:
  assert(zdMin <= z0);
  //assert(zdMin <= zD1); // not true anymore since used BreakPointCalulator
  // io MinimumCalculator
	
	assert(dMin >= 0);
	
	SolverExpr dnFunctionExpr
#ifdef USE_CPLEX_NATIVE
	(*global_env_)
#endif
	;
	
	if (D1 > dMin) {
	
		if (dMin > 0) { // only dnFunction when something left of dMin
			dnFunctionExpr += z0 + (zdMin - z0)/(dMin - 0) * (xPlusYExpr - 0);
			
			// now imagine a V shape described by these 3 points.
			addConstr(dnFunctionExpr, "<=", z,
								xPlusYName + "_sum_convex_max_robust_up_function",
								doUpdate);
			// function row (1)	
		}
		
		SolverExpr upFunctionExpr
#ifdef USE_CPLEX_NATIVE
		(*global_env_)
#endif
		;
		upFunctionExpr += 
		zdMin + (zD1 - zdMin)/(D1 - dMin) * (xPlusYExpr - dMin);
		addConstr(upFunctionExpr, "<=", z,
							xPlusYName + "_sum_convex_max_robust_dn_function",
							doUpdate);
		// function row (1)	
		
	} else {
		//cerr << D1 << " = D1 <= dMin = " << dMin << endl;
		assert(D1==dMin);
		assert(D1!=0);
		assert(zD1 <= z0); // dn slope
		dnFunctionExpr += 
		z0 + (zD1 - z0)/(D1 - 0) * (xPlusYExpr - 0);
		addConstr(dnFunctionExpr, "<=", z,
							xPlusYName + "_sum_convex_max_robust_dn_only_function",
							doUpdate);		
	}
}


string Solver::getNameLoHi(int & xLo, int & xHi, const SolverVar * x) const {
	double xLoDouble = getLowerBound(*x);
	xLo = (int)xLoDouble;
	assert(xLo == xLoDouble);
	
	double xHiDouble = getUpperBound(*x);
	xHi = (int)xHiDouble;
	assert(xHi == xHiDouble);
	
	string xName = getName(*x);
	
	assert( 0 <= xLo);
	assert( 0 <= xHi);
	assert(xLo <= xHi);
	
	return xName;
}

void Solver::setStep(const SolverVar * x, unsigned int step) {
	assert(stepMap_.count(x)==0);
	stepMap_[x] = step;
}

unsigned int Solver::getStep(const SolverVar * x) const {
	assert(stepMap_.count(x)==1);
	return stepMap_.find(x)->second;
}

const SolverExpr & Solver::getNullExpr() {
	return *nullExpr_;
}

const SolverExpr & Solver::getOneExpr() {
	return *oneExpr_;
}

const SolverExpr & Solver::getMinusOneExpr() {
	return *minusOneExpr_;
}


void Solver::setDeterministic() {
	assert(false); // child solver needs to implement this
	// should become pure virtual soon
}

void Solver::setMipFocus(unsigned int value) {
	//assert(false); // FIXME
  cerr << "setMipFocus: child solver needs to implement this" << endl;
	cerr << "should become pure virtual soon" << endl;
}

void Solver::setSymmetryDetectionLevel(int level) {
	assert(false); // child solver needs to implement this
	// should become pure virtual soon
}

void Solver::improveStartGap(double gap) {
	assert(false); // child solver needs to implement this
	// should become pure virtual soon
}

void Solver::improveStartTime(double time) {
	assert(false); // child solver needs to implement this
	// should become pure virtual soon
}

void Solver::setHeuristics(double level) {
	assert(false); // child solver needs to implement this
	// should become pure virtual soon
}

void Solver::setVarBranch(int value) {
	assert(false); // child solver needs to implement this
	// should become pure virtual soon
}

////////////////// Cuts Control ///////////////
void Solver::setCutsLevel(int level)  {
	assert(-1 <= level);
	assert(level <= 3);
	
	assert(false); // child solver needs to implement this
	// should become pure virtual soon}
}

void Solver::setCliqueCutsLevel(int level) {
	assert(-1 <= level);
	assert(level <= 2);
	
	assert(false); // child solver needs to implement this
	// should become pure virtual soon}
}

void Solver::setCoverCutsLevel(int level) {
	assert(-1 <= level);
	assert(level <= 2);
	
	assert(false); // child solver needs to implement this
	// should become pure virtual soon}
}

void Solver::setFlowCoverCutsLevel(int level)  {
	assert(-1 <= level);
	assert(level <= 2);
	
	assert(false); // child solver needs to implement this
	// should become pure virtual soon}
}

void Solver::setFlowPathCutsLevel(int level) {
	assert(-1 <= level);
	assert(level <= 2);
	
	assert(false); // child solver needs to implement this
	// should become pure virtual soon}
}

void Solver::setBranchingMethod(int method) {
	assert(-1 <= method);
	assert(method <= 3);
	
	assert(false); // child solver needs to implement this
	// should become pure virtual soon}  
}

////////////////// End Cuts Control ///////////////

Solver::~Solver() {
  cout << "in ~Solver()" << endl;  
}
