#ifndef CPLEX_EXPRESSION_H
#define CPLEX_EXPRESSION_H

#include <iostream>
#include <assert.h>

#include "HasSolver.h"

/*
#define ILOUSESTL

using namespace std;
#include <ilcplex/ilocplex.h>
*/

#ifdef USE_CPLEX_NATIVE

class CplexExpression : public IloNumExpr {
public:
  CplexExpression();
  CplexExpression(const IloNumExpr & iloNumExpr);
  //CplexExpression & operator = (const IloNumExpr & iloNumExpr);
};

#endif // USE_CPLEX_NATIVE

#endif // CPLEX_EXPRESSION_H