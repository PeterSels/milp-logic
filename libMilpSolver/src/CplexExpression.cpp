#include "CplexExpression.h"

#ifdef USE_CPLEX_NATIVE

extern IloEnv * global_env_;

CplexExpression::CplexExpression() 
: IloNumExpr(*global_env_)
{
}


CplexExpression::CplexExpression(const IloNumExpr & iloNumExpr)
: IloNumExpr(*global_env_)
{
}

/*
CplexExpression & CplexExpression::operator = (const IloNumExpr & iloNumExpr) {
  //this->set  env_ = global_env_;
}
*/

#endif // USE_CPLEX_NATIVE
