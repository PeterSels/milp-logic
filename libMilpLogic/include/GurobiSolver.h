#ifndef GUROBI_SOLVER_H
#define GUROBI_SOLVER_H

#include <string>

#include "Solver.h"

#ifdef USE_GUROBI_NATIVE

class GurobiSolver : public Solver {
public:
  GurobiSolver(double maxGetLicenseSeconds, 
               double maxSolveSeconds);

  // setting up model
  void resetModel();
  void deleteModelAndEnv();

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

  
  
  
  //////////// fast constraint addition without pushing onto vector //////////
  // nor doing solver_->update
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
  /////////////////////////////////////////////////////////////////////////////
  
  void addSos1SolverSpecific(
    const std::string & name,
    const std::vector<SolverVar> & vars, 
    const std::vector<double> & coeffs);

	virtual void update(); // Specifically for Gurobi really
	
  std::string getName(const SolverVar & var) const;
  double getLowerBound(const SolverVar & var) const;
  double getUpperBound(const SolverVar & var) const;
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

  unsigned int getNumberOfSets() const;
  unsigned int getNumberOfSetMembers() const;

  double getValueOf(const SolverExpr & expr) const;
  double getValueOf(const SolverVar & var) const;
  double getObjVal() const;
  double getObjBound() const;
  double getMipGap() const;
  
  void calcAnIIS() const;
  void reportAnIISTo(std::ostream & ostr) const;
  
  ~GurobiSolver();

  static char compStringToChar(
    const std::string & comp, const std::string & funcName);
  
  double getInfinity() const;
    
	void setDeterministic();
	void setMipFocus(unsigned int value);
	void setSymmetryDetectionLevel(int level);
	void improveStartGap(double gap);
	void improveStartTime(double time);
  void setTimeLimit(double time);
	void setHeuristics(double level);
  void setVarBranch(int value);
	
	void setCutsLevel(int level);
	void setCliqueCutsLevel(int level);
	void setCoverCutsLevel(int level);
	void setFlowCoverCutsLevel(int level);
	void setFlowPathCutsLevel(int level);
  
	void setBranchingMethod(int method);

  void setIntFeasTol(double value);
  
  std::string getVersionString() const;

  
private:
  void setNThreads(int nThreads);

  GRBEnv * env_;
};

#endif // USE_GUROBI_NATIVE

#endif // GUROBI_SOLVER_H
