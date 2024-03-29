#ifndef XPRESS_SOLVER_H
#define XPRESS_SOLVER_H

#include <string>

#include "Solver.h"

#ifdef USE_XPRESS_NATIVE

class XpressSolver : public Solver {
public:
  XpressSolver(double maxGetLicenseSeconds, 
               double maxSolveSeconds);

  // setting up model
  void resetModel();

  const SolverVar & addLpVar(
    double lo, double hi, double objCoef, const std::string & name,
    bool doUpdate=true);
  const SolverVar & addIntVar(
    int lo, int hi, double objCoef, const std::string & name,
    bool doUpdate=true);
  const SolverVar & addBinVar(
    double objCoef, const std::string & name,
    bool doUpdate=true);

  const SolverExpr & addExpr(
    const SolverExpr & expr, const std::string & name, bool doUpdate=true);

  const SolverConstr & addConstr(
    const SolverVar & lhs, const std::string & comp, const SolverVar & rhs,
    const std::string & name, bool doUpdate=true);
  const SolverConstr & addConstr(
    const SolverExpr & lhs, const std::string & comp, const SolverVar & rhs,
    const std::string & name, bool doUpdate=true);
  const SolverConstr & addConstr(
    const SolverExpr & lhs, const std::string & comp, const SolverExpr & rhs,
    const std::string & name, bool doUpdate=true);

  
  // fast add constraint methods
  void fastAddConstr(const SolverVar & lhs, 
                     const std::string & comp, 
                     const SolverVar & rhs, 
                     const std::string & name);
  void fastAddConstr(const SolverExpr & lhs, 
                     const std::string & comp, 
                     const SolverVar & rhs, 
                     const std::string & name);
  void fastAddConstr(const SolverExpr & lhs, 
                     const std::string & comp, 
                     const SolverExpr & rhs, 
                     const std::string & name);
  //
  
  
  
  void addSos1SolverSpecific(
    const std::string & name,
    const std::vector<SolverVar> & vars, 
    const std::vector<double> & coeffs);

	virtual void update();

  std::string  getName(const SolverVar & var) const;
  double getUpperBound(const SolverVar & var) const;
  double getLowerBound(const SolverVar & var) const;
	void setLowerBound(SolverVar & var, double value);
	void setUpperBound(SolverVar & var, double value);

  // reporting/debugging
  void exportModelAsLpFile(const std::string & fileNamePrefix) const;
  void exportModelAsMpsFile(const std::string & fileNamePrefix) const;
  void exportModelAsRewFile(const std::string & fileNamePrefix) const;

  // solving
  void setMinimize();
  void setMaximize();
  void setStartValueOf(SolverVar & var,
                       double startValue) const;  
  bool solve(double gap, int nThreads, double maxSolveSeconds);

  // get results
  unsigned int getNumberOfRows() const;
  unsigned int getNumberOfColumns() const;
  unsigned int getNumberOfNonZeroes() const;

  double getValueOf(const SolverExpr & expr) const;
  double getValueOf(const SolverVar & var) const;
  double getObjVal() const;
  double getMipGap() const;

  unsigned int getNumberOfSets() const;
  unsigned int getNumberOfSetMembers() const;

  void calcAnIIS() const;
  void reportAnIISTo(std::ostream & ostr) const;

  double getInfinity() const;  
  
  std::string getVersionString() const;

  ~XpressSolver();

  static char compStringToChar(
    const std::string & comp, const std::string & funcName);
private:
  void setNThreads(int nThreads);

  SolverExpr objFunction_;
};

#endif // USE_XPRESS_NATIVE

#endif // XPRESS_SOLVER_H
