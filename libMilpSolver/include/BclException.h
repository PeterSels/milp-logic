#ifndef BCL_EXCEPTION_H
#define BCL_EXCEPTION_H

#include <iostream>
#include <string>

#include "HasSolver.h"
  // this includes "xprb_cpp.h"

#ifdef USE_XPRESS_NATIVE

class BclException {
public:
  std::string msg;
  int code;
  BclException(int c, const char *m);
};

typedef BclException SolverException;

// User error handling function
void XPRB_CC usererror(
  xbprob * prob, void * vp, int num, int type, const char * t);
void XPRB_CC userprint(
  xbprob * prob, void * vp, const char * msg);
void modexpl3(dashoptimization::XPRBprob &p);

#endif // USE_XPRESS_NATIVE

#endif // BCL_EXCEPTION_H
