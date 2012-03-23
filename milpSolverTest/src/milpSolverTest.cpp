#include <iostream>
#include <assert.h>
#include <vector>

using namespace std;

#include "HasSolver.h"
#include "Solver.h"
#include "GurobiSolver.h"
#include "XpressSolver.h"
#include "CplexSolver.h"

// User can turn on one of the below defines 
// by setting it to true to activate the respective test.
#define TEST_SOS (true)
#define TEST_BIN_LE_EQS (false)
#define TEST_BIN_EQ_EQS (true)

#ifdef USE_CPLEX_NATIVE
extern IloEnv * global_env_;
#endif

Solver * solver_;
/*
double f(int i) {
  if (i < 10) {
    return 100 - i;
  } else {
    return 2 * i;
  }
}
*/

double f(const vector<double> & parameters, double i) {
	assert(parameters.size()==1);
	double xMin = parameters[0];
  if (i < xMin) {
    return 100 - i;
  } else {
    return 2 * i;
  }	
}


void resetModel(unsigned int maxGetLicenseSeconds, 
								unsigned int maxSolverSeconds) {
	cout << "resetModel" << endl;
  if (solver_==0) {
#ifdef USE_GUROBI_NATIVE
    solver_ = new GurobiSolver();
#endif
#ifdef USE_XPRESS_NATIVE
    solver_ = new XpressSolver();
#endif
#ifdef USE_CPLEX_NATIVE
    solver_ = new CplexSolver(maxGetLicenseSeconds, maxGetLicenseSeconds);
#endif
  }
  assert(solver_!=0);
  solver_->resetModel();
  cout << endl;
}

int main(int argc, char * argv[]) {
  cout << "milpSolverTest" << endl;
	
  solver_ = 0;
	resetModel(1*60, 5*60); // arguments used by cplex only
  
  if (TEST_SOS) {
		int i=1;
    cout << "<<<<<<<<<<<<<<<<<< " << i << " >>>>>>>>>>>>>>" << endl;
    if (i==0) {
      bool min = false; // max if false
      unsigned int lo = 10;
      unsigned int hi = 99;
      const SolverVar x = solver_->addIntVar(lo, hi, 1, "x");
			
      solver_->addSos1(x);
      
      if (min) {
        solver_->setMinimize();      
      } else {
        solver_->setMaximize();
      }
			
			solver_->exportModelAsLpFile("sosx");			
			
			double gap = 0.0;
      bool solved = solver_->solve(gap);
      
      assert(solved);
      double objSol_ = solved ? solver_->getObjVal() : 0;
      cout << "goal function value = " << objSol_ << endl;   
      double xVal = solver_->getValueOf(x);
      cout << "x = " << xVal << endl;
      if (min) {
        assert(xVal == lo);
        assert(objSol_ == lo);
      } else {
        assert(xVal == hi);
        assert(objSol_ == hi);
      }
    }      
    
    if (i==1) {
      // default is cost equals sum of x-variable values
      unsigned int wx = 1;
      unsigned int wy = 0;
      bool realCost = true; // if true: use y variables value as cost
      if (realCost) {
        wx = 0;
        wy = 1;
      }
			
      const SolverVar x = solver_->addIntVar(0, 99, wx, "x");
      const SolverVar y = solver_->addIntVar(0, 99, wy, "y");
			vector<double> parameters;
			parameters.push_back(10);
			
			solver_->addBinVarsFor(&x, 1, true);
			solver_->addBinConvexityAndReferenceRowsFor(&x);

			solver_->addBinVarsFor(&y, 1, true);
			solver_->addBinConvexityAndReferenceRowsFor(&y);
			
      solver_->addSos1(x, y, f, parameters/*, 1*/);
      
      solver_->setMinimize();
			
			//solver_->exportModelAsLpFile("sosxyf");						
			
			double gap = 0.0;
      bool solved = solver_->solve(gap);
      
      assert(solved);
      double objSol_ = solved ? solver_->getObjVal() : 0;
      cout << "goal function value = " << objSol_ << endl;   
      double xVal = solver_->getValueOf(x);
      int xIntVal = (int)xVal;
      assert(xIntVal == xVal);
      cout << "x = " << xVal << endl;
      double yVal = solver_->getValueOf(y);
      cout << "y = " << yVal << endl;
      cout << "check:f(x)=" << f(parameters, xIntVal) << ", y=" << yVal << endl;
      
      assert(xVal == 10);
      assert(yVal == 20);
      if (realCost) {
        assert(objSol_ == 20); // yVal
      } else {
        assert(objSol_ == 10); // xVal
      }
    } 
		
  } else if (TEST_BIN_LE_EQS) {
		
    const SolverVar b0 = solver_->addIntVar(10, 40, 0, "b0");
    int m0 = 5;
    const SolverVar s0 = solver_->addIntVar( 0, 10, 1, "s0");
    const SolverVar e0 = solver_->addIntVar(10, 40, 0, "e0");
		
    const SolverVar b1 = solver_->addIntVar(10, 50, 0, "b1");
    int m1 = 7;
    const SolverVar s1 = solver_->addIntVar( 0, 10, 1, "s1");
    const SolverVar e1 = solver_->addIntVar(10, 50, 0, "e1");
		
    //SolverExpr lhs0 = b0 + m0 + s0; // worked for all not for Gurobi v4.0
    SolverExpr lhs0
#ifdef USE_CPLEX_NATIVE
      (*global_env_)
#endif
    ;
    lhs0 += b0;
		lhs0 += m0;
		lhs0 += s0;
    solver_->addConstr(lhs0, "==", e0, "c0");
		
    SolverExpr lhs1
#ifdef USE_CPLEX_NATIVE
		(*global_env_)
#endif
    ;
    lhs1 += b1;
		lhs1 += m1;
		lhs1 += s1;
    solver_->addConstr(lhs1, "==", e1, "c1");
		
		
		SolverExpr le0LhsExpr
#ifdef USE_CPLEX_NATIVE
    (*global_env_)
#endif
		;
		le0LhsExpr += b0;
		le0LhsExpr += 3;
		
    SolverVar le0 = solver_->addLessOrEqualBinVar(0, 
																									le0LhsExpr, 10+3, 40+3,
																									b1,   10,   50,
																									1,
																									"le0_0before1"
																									);
		
		SolverExpr le1LhsExpr
#ifdef USE_CPLEX_NATIVE
    (*global_env_)
#endif
		;
		le1LhsExpr += b1;
		le1LhsExpr += 3;
    SolverVar le1 = solver_->addLessOrEqualBinVar(0, 
																									le1LhsExpr, 10+3, 50+3, 
																									b0,   10,   40,
																									1,
																									"le1_1before0"
																									);
    solver_->addNegationConstr(le0, le1, "neg");
		
		int sense = +1; // choose -1 or +1
    assert((sense==-1) || (sense==+1));
    if (sense == -1) {
      solver_->setMinimize();
    } else {
      assert(sense == +1);
      solver_->setMaximize();
    }
    solver_->exportModelAsLpFile("le");
		
		double gap = 0.0;
    bool solved = solver_->solve(gap);
    assert(solved);
		
    double objSol_ = solved ? solver_->getObjVal() : 0;
    cout << "goal function value = " << objSol_ << endl;   
    if (sense==-1) {
      assert(objSol_==0.0);
    } else {
      assert(objSol_==20.0);
    }
		
    double b0Val = solver_->getValueOf(b0);
    double s0Val = solver_->getValueOf(s0);
    double e0Val = solver_->getValueOf(e0);
    cout << "(b,s,e)0 = " << "(" << b0Val 
      << "," << s0Val << "," << e0Val << ")" << endl;
    assert(b0Val + m0 + s0Val == e0Val);
		
    double b1Val = solver_->getValueOf(b1);
    double s1Val = solver_->getValueOf(s1);
    double e1Val = solver_->getValueOf(e1);
    cout << "(b,s,e)1 = " << "(" << b1Val 
      << "," << s1Val << "," << e1Val << ")" << endl;
    assert(b1Val + m1 + s1Val == e1Val);
		
    int le0Val = (int)solver_->getValueOf(le0);
    int le1Val = (int)solver_->getValueOf(le1);
    cout << "(le0,le1)=" << "(" << le0Val 
      << "," << le1Val << ")" << endl;
		
    // It could be the other way around too
    if (le0Val==1) {
      assert(le1Val==0);
    } else {
      assert(le0Val==0);
      assert(le1Val==1);
    }	
		
	} else if (TEST_BIN_EQ_EQS) {
		
    const SolverVar xVar = solver_->addIntVar(10, 100, 0, "x");
    SolverExpr thirtyFiveExpr
#ifdef USE_CPLEX_NATIVE
      (*global_env_)
#endif
    ;
    thirtyFiveExpr += 35;
    SolverVar eq = solver_->addEqualBinVar(1, 
																					 xVar,           10, 100,
																					 thirtyFiveExpr, 10, 100,
                                           1,
																					 "x_eq_35");
    solver_->setMaximize();
    solver_->exportModelAsLpFile("eq");
		
		double gap = 0.0;
    bool solved = solver_->solve(gap);
    assert(solved);
		
    double objSol_ = solved ? solver_->getObjVal() : 0;
    cout << "goal function value = " << objSol_ << endl;   
    cout << "x value             = " << solver_->getValueOf(xVar) << endl;	
	}
	
	unsigned int r = solver_->getNumberOfRows();
  unsigned int c = solver_->getNumberOfColumns();
  unsigned int nz = solver_->getNumberOfNonZeroes();
  cout << "(r,c,nz) = (" << r << ", " << c << ", " << nz << ")" << endl;
  unsigned int ns = solver_->getNumberOfSets();
  cout << "ns = " << ns << endl;
	
	return 0;
}
