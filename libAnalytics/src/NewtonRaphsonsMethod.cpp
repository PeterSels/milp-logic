#include <math.h>
#include <iostream>

#include "NewtonRaphsonsMethod.h"
#include "libAnalyticsDefinitions.h"
  // for TOLERANCE

using namespace std;

NewtonRaphsonsMethod::NewtonRaphsonsMethod(/* f, f'*/) {
}

// method taken from sources:
// http://mathworld.wolfram.com/NewtonsMethod.html
// http://en.wikipedia.org/wiki/Newton's_method
double NewtonRaphsonsMethod::findZero(
  double (*function)(double, double, double),
  double (*derivative)(double, double, double),
  double guessValue, double a, double b) {

  double x  = guessValue;
  double error;
  do {
    double fx = function(x, a, b);
    cout << "x=" << x << ", fx=" << fx << endl;
    double dfx = derivative(x, a, b);
    x -= fx/dfx;
    error = fabs(fx-0);
  } while (error > NEWTON_TOLERANCE);

  return x;
}

NewtonRaphsonsMethod::~NewtonRaphsonsMethod() {
}
