#include <iostream>
#include <sstream>
#include <math.h>
#include <assert.h>
#include <limits>
#include <cstdlib>
  // for srand and rand
#include <iomanip>

#include "BiExponentialConvolution.h"
#include "Exponential.h"

using namespace std;

/* a is part of the normalized distribution p(x) = a * e^(-a x)
   b is part of the normalized distribution p(y) = b * e^(-b y)
*/
BiExponentialConvolution::BiExponentialConvolution(double a, double b)
: a_(a), b_(b) {
  assert(a_  > 0);
  assert(b_  > 0);
  assert(a_ != b_);
}

double BiExponentialConvolution::eval(double D) const {
  assert(D >= 0);
  double result0 = (a_ * b_) / (a_ - b_)  * (exp(-a_ * D) - exp(-b_ * D));
  return result0;
}

double BiExponentialConvolution::valueForDis0() const {
	double result = 0.0;
	const double eval0 = eval(0);
	assert(eval0==result);
	return result;
}

double BiExponentialConvolution::surfaceUnderCurveForPositiveX() const {
	return 1;
}

double BiExponentialConvolution::expectedValueForPositiveX() const {
  double result = (a_ + b_)/(a_ * b_);
	return result;
}

double BiExponentialConvolution::probabilityThatSumIsLessThan(double D) const {
  double result = 1 - probabilityThatSumIsGreaterThan(D);
	return result;
}

double BiExponentialConvolution::probabilityThatSumIsGreaterThan(double D) const{
  double result = a_ / (b_ - a_)  * (exp(-a_ * D) - exp(-b_ * D)) + exp(-a_ * D);  
	return result;
}

double BiExponentialConvolution::getRandomSample() const {
  Exponential aExp(a_, -a_);
  Exponential bExp(b_, -b_);
  double sample = aExp.getRandomSample() + bExp.getRandomSample();
  assert(sample >= 0);
  return sample;
}

string BiExponentialConvolution::toString() const {
	ostringstream sstrm;
	sstrm 
    << "BiExponentialConvolution(a_ = " << a_ 
    << ", b_ = " << b_;
  sstrm  << ")";
	return sstrm.str();
}

ostream & operator << (ostream & ostrm, const BiExponentialConvolution & biExpConv) {
	ostrm << biExpConv.toString();
	return ostrm;
}

BiExponentialConvolution::~BiExponentialConvolution() {
}
