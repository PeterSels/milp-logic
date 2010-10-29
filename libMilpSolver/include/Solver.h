#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include <string>

#include "HasSolver.h"

#undef USE_SOS_VECTOR

class Solver {
protected:
  SolverModel * model_;
  std::vector<SolverVar> varVector_;
  std::vector<SolverExpr> exprVector_;
  std::vector<SolverConstr> constrVector_;
#ifdef USE_SOS_VECTOR
  std::vector<SolverSos> sosVector_;
#endif
  bool solved_;
public:
  Solver();

  // setting up model
  virtual void resetModel() = 0;

  //// layer 1:
  virtual const SolverVar & addLpVar(
    double lo, double hi, double objCoef, const std::string & name) = 0;
  virtual const SolverVar & addIntVar(
    int lo, int hi, double objCoef, const std::string & name) = 0;
  virtual const SolverVar & addBinVar(
    double objCoef, const std::string & name) = 0;

  virtual const SolverExpr & addExpr(
    const SolverExpr & expr, const std::string & name) = 0;

  virtual const SolverConstr & addConstr(
    const SolverVar & lhs, const std::string & comp, const SolverVar & rhs,
    const std::string & name) = 0;
  virtual const SolverConstr & addConstr(
    const SolverExpr & lhs, const std::string & comp, const SolverVar & rhs, 
    const std::string & name) = 0;
  virtual const SolverConstr & addConstr(
    const SolverExpr & lhs, const std::string & comp, const SolverExpr & rhs,
    const std::string & name) = 0;


  //// layer 2:
  // a = (b and c)
  virtual const SolverVar /*&*/ addConjunctionBinVar(
    double objCoef, 
    const SolverVar & conjunctionBinVarB, 
    const SolverVar & conjunctionBinVarC, 
    const std::string & name);
  virtual void addConjunctionConstr(
    const SolverVar & conjunctionBinVarA,
    const SolverVar & conjunctionBinVarB,
    const SolverVar & conjunctionBinVarC,
    const std::string & name);

  // a = (b or c)
  virtual const SolverVar /*&*/ addDisjunctionBinVar(
    double objCoef,
    const SolverVar & disjunctionBinVarB,
    const SolverVar & disjunctionBinVarC,
    const std::string & name);
  virtual void addDisjunctionConstr(
    const SolverVar & disjunctionBinVarA,
    const SolverVar & disjunctionBinVarB,
    const SolverVar & disjunctionBinVarC,
    const std::string & name);

  // a = !b
  virtual void addNegationConstr(
    const SolverVar & binVarA,
    const SolverVar & binVarB,
    const std::string & name);


  // a = (b <= c)
  const SolverVar addLessOrEqualBinVar(
    double objCoef,
    const SolverExpr & lhsExpr,
    double lhsLowerBound,  double lhsUpperBound,
    const SolverExpr & rhsExpr,
    double rhsLowerBound,  double rhsUpperBound,
    double unit,
    const std::string & name);
  void addLessOrEqualConstr(
    const SolverVar & leBinVar,                                   
    const SolverExpr & lhsExpr,
    double lhsLowerBound,  double lhsUpperBound,
    const SolverExpr & rhsExpr,
    double rhsLowerBound,  double rhsUpperBound,
    double unit,
    const std::string & name);

  //////////////////----
  const SolverVar addLessOrEqualBinVar(double objCoef,
																			 const SolverExpr & lhsExpr,
																			 double lhsLowerBound,  double lhsUpperBound,
																			 double rhs,
																			 double unit,
																			 const std::string & name);
	
  void addLessOrEqualConstr(const SolverVar & leBinVar,
														const SolverExpr & lhsExpr,
														double lhsLowerBound,  double lhsUpperBound,
														double rhs, 
														double unit,
														const std::string & name);
  //////////////////----

  // a = (b == c)
  const SolverVar addEqualBinVar(
    double objCoef,
    const SolverExpr & lhsExpr,
    double lhsLowerBound,  double lhsUpperBound,
    const SolverExpr & rhsExpr,
    double rhsLowerBound,  double rhsUpperBound,
    double unit,
    const std::string & name);																						 
  void addEqualConstr(
    const SolverVar & leBinVar,                                   
    const SolverExpr & lhsExpr,
    double lhsLowerBound,  double lhsUpperBound,
    const SolverExpr & rhsExpr,
    double rhsLowerBound,  double rhsUpperBound,
    double unit,
    const std::string & name);
	
  //////////////
  const SolverVar addEqualBinVar(
    double objCoef,
    const SolverExpr & lhsExpr,
    double lhsLowerBound,  double lhsUpperBound,
    double rhs,
    double unit,
    const std::string & name);
  void addEqualConstr(
    const SolverVar & eqBinVar,
    const SolverExpr & lhsExpr,
    double lhsLowerBound,  double lhsUpperBound,
    double rhs,
    double unit,
    const std::string & name);
  //////////////


  // a => b
  void addImplication(
    const SolverVar & binVarA, 
    const SolverVar & binVarB, 
    const std::string & name);
  // a <=> b
  void addEquivalence(
    const SolverVar & binVarA,
    const SolverVar & binVarB,
    const std::string & name);


  // special ordered set type 1
  /*
  virtual void addSos1(
    const std::vector<const SolverVar *> & vars);
  */
  virtual //const SolverSos & 
  void addSos1SolverSpecific(
    const std::string & name,
    const std::vector<SolverVar> & vars, 
    const std::vector<double> & coeffs) = 0;
  
  void addSos1(const SolverVar & x);
  void addSos1(const SolverVar & x, const SolverVar & y, double (*fPtr)(int));


  virtual std::string  getName(const SolverVar & var) const = 0;
  virtual double getLowerBound(const SolverVar & var) const = 0;
  virtual double getUpperBound(const SolverVar & var) const = 0;

  // reporting/debugging
  virtual void exportModelAsLpFile(const std::string & fileName) const = 0;
  virtual void exportModelAsMpsFile(const std::string & fileName) const = 0;

  // solving
  virtual void setMinimize() = 0;
  virtual void setMaximize() = 0;
  virtual bool solve(double gap) = 0;

  // get results
  virtual unsigned int getNumberOfRows() const = 0;
  virtual unsigned int getNumberOfColumns() const = 0;
  virtual unsigned int getNumberOfNonZeroes() const = 0;

  virtual unsigned int getNumberOfSets() const = 0;
  virtual unsigned int getNumberOfSetMembers() const = 0;

  virtual double getValueOf(const SolverExpr & expr) const = 0;
  virtual double getValueOf(const SolverVar & var) const = 0;
  virtual double getObjVal() const = 0;

  virtual void calcAnIIS() const = 0;
  virtual void reportAnIISTo(std::ostream & ostr) const = 0;
  
  virtual double getInfinity() const = 0;
  
  std::string lpConvert(const std::string & name);

  virtual ~Solver();
};

#endif // SOLVER_H

