#ifndef HAS_SOLVER_H
#define HAS_SOLVER_H

// You should change this file according to the solvers your have available.
// You can make this conditional on the host system or/and OS type.
// The #ifdefs HAS_<solver> in our code are such that
// you can only define one HAS_<solver> at a time.
// If you define multiple ones, you will get multiple defined variables,
// which will prevent you from building this library.

#undef USE_XPRESS_NATIVE
#undef USE_GUROBI_NATIVE
#undef USE_CPLEX_NATIVE
#undef USE_CBC
#undef USE_OSI

// Currently only the 3 modes:
// USE_GUROBI_NATIVE, USE_CPLEX_NATIVE and USE_XPRESS_NATIVE
// are tested an working fine.
// More work is needed to make USE_CBC or USE_OSI work.

#ifdef _WIN32
#include <iostream>
// to get namespace std definition only
// define only one!
//#define USE_CPLEX_NATIVE
#define USE_GUROBI_NATIVE
//#define USE_XPRESS_NATIVE
//#define USE_OSI
//#define USE_CBC
#else
//#define USE_OSI
//#define USE_CPLEX_NATIVE
#define USE_GUROBI_NATIVE
//#define USE_XPRESS_NATIVE
// XPRESS not available for Mac OS X
#endif

#ifdef USE_XPRESS_NATIVE
#include <string>
// #include<string> is needed to define NULL for xprb_cpp.h
// xprb_cpp.h should in fact include this itself!
#include "xprb_cpp.h"
#include "xprb.h"
#include "xprs.h"
typedef dashoptimization::XPRBprob SolverModel;
typedef dashoptimization::XPRBvar SolverVar;
typedef dashoptimization::XPRBlinExp SolverExpr;
typedef dashoptimization::XPRBctr SolverConstr;
typedef dashoptimization::XPRBsos SolverSos;
#include "BclException.h"
#define SOLVER_NAME "Xpress"
#endif

#ifdef USE_GUROBI_NATIVE
#include "gurobi_c++.h"
typedef GRBModel SolverModel;
typedef GRBVar SolverVar;
typedef GRBLinExpr SolverExpr;
typedef GRBConstr SolverConstr;
typedef GRBException SolverException;
typedef GRBSOS SolverSos;
#define SOLVER_NAME "Gurobi"
#endif

#ifdef USE_CPLEX_NATIVE
#define ILOUSESTL
using namespace std;
#include <ilcplex/ilocplex.h>
#include "CplexExpression.h"
typedef IloModel SolverModel;
typedef IloNumVar SolverVar;
typedef IloNumExpr SolverExpr;
typedef IloConstraint SolverConstr;
typedef IloException SolverException;
typedef IloSOS1 SolverSos;
typedef IloEnv SolverEnv;
#define SOLVER_NAME "Cplex"
#endif

#ifdef USE_CBC
#include "OsiSolverInterface.hpp"
#include "OsiCbcSolverInterface.hpp"
//#include "OsiSymSolverInterface.hpp"
//#include "OsiVolSolverInterface.hpp"
//#include "OsiDylpSolverInterface.hpp"
//#include "OsiClpSolverInterface.hpp"
#include "CbcModel.hpp"
typedef CBCModel SolverModel;
typedef CBCVar SolverVar;
typedef CBCLinExpr SolverExpr;
typedef CBCConstr SolverConstr;
typedef CBCException SolverException;
typedef CBCSOS SolverSos;
#define SOLVER_NAME "Cbc"
#endif

#ifdef USE_OSI
#include "flopc.hpp"
#include "MP_Model.hpp"
#include "MP_variable.hpp"
#include "MP_expression.hpp"

typedef flopc::MP_variable* SolverVar;
typedef flopc::MP_expression* SolverExpr;
typedef flopc::MP_constraint* SolverConstr;
typedef flopc::MP_model* SolverModel;
#define SOLVER_NAME "Osi"
#endif

#endif // HAS_SOLVER_H
