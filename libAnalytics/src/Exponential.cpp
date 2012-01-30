/*
 *  Exponential.cpp
 *  ScheduleModelBuilder
 *
 *  Created by Peter Sels on 13/07/09.
 *
 */

#include <iostream>
#include <sstream>
#include <math.h>
#include <assert.h>
#include <limits>
#include <cstdlib>
  // for srand and rand
#include <iomanip>

#include "Exponential.h"
#include "NewtonRaphsonsMethod.h"

using namespace std;

Exponential::Exponential(double factor, double exponent) {
  init(factor, exponent);
}

void Exponential::init(double factor, double exponent) {
  factor_ = factor;
  exponent_ = exponent;
  setNormalizeLambda();
  setNormalizeFactor();
}

double Exponential::eval(double x) const {
	return factor_ * exp(exponent_ * x);
}

double Exponential::valueForXis0() const {
	double result = factor_;
	const double eval0 = eval(0);
	assert(eval0==result);
	return result;
}

double Exponential::surfaceUnderCurveForPositiveX() const {
	double result;
	if (exponent_<0) {
		result = fabs(factor_) / -exponent_;
	} else {
		result = numeric_limits<double>::max();
	}
	return result;
}

void Exponential::setNormalizeLambda() {
  normalizeLambda_ = 0.0;
  if (exponent_<0) {
    normalizeLambda_ = -exponent_;
  }
}

void Exponential::setNormalizeFactor() {
  normalizeFactor_ = 1.0;
  if (exponent_<0) {
    normalizeFactor_ = surfaceUnderCurveForPositiveX();
  }
}

double Exponential::expectedValueForPositiveX() const {
	double result;
	if (exponent_<0) {
    result = 1 / -exponent_;
	} else {
		result = numeric_limits<double>::max();
	}
	return result;
}

double Exponential::probabilityThatXIsSmallerThan(double X) const {
  double result;
  if (X <= 0) { // we suppose only positive X are possible
    result = 0;
  } else {
    if (exponent_ < 0) {
    	result = (1 - (exp(exponent_ * X)));
    } else {
      result = 0.0;
    }
  }
	return result;
}

double Exponential::probabilityThatXIsGreaterThan(double X) const{
  double result;
  if (X <= 0) { // we suppose only positive X are possible
    result = 0;
  } else {
    if (exponent_ < 0) {
    	result = (exp(exponent_ * X));
    } else {
      result = 1.0;
    }
  }
	return result;
}

double Exponential::getRandomSample() const {
  assert(exponent_<0);
  // Note that the factor_ does not play a role here.
  double linearRandom;
  do {
    linearRandom = (double)rand() / (double)RAND_MAX;
  } while (linearRandom==0);
  // linearRandom calculates p(x), while we need to know x
  // p(x) = 0 should in fact never occur, hence the while loop

  //cerr << "linearRandom = " 
  //  << setiosflags(ios::fixed) << setprecision(6)
  //  << linearRandom << endl;
  double logLinearRandom;
  //if (linearRandom == 0) {
  //  logLinearRandom = 0.0;
  //} else {
    logLinearRandom = log(linearRandom);
  //}
  double sample = -1/normalizeLambda_ * logLinearRandom;
  assert(sample >= 0);
  //cerr << "delay sample = " << sample << " from distro with lambda = " << normalizeLambda_ << endl;
  if (sample < 0) {
    cerr << "ERROR sampling" << endl;
    assert(false);
  }
  if (sample > 10000) { // quite unlikely, but not really impossible
    cerr << "ERROR sampling" << endl;
    assert(false);
  }
  return sample;
}

string Exponential::toString() const {
	ostringstream sstrm;
	sstrm 
    << "Exp(" << factor_ 
    << "," << exponent_;
  if (exponent_<0) {
    sstrm << ",nF=" << normalizeFactor_;
    sstrm << ",l=" << normalizeLambda_;
  }
  sstrm  << ")";
	return sstrm.str();
}

ostream & operator << (ostream & ostrm, const Exponential & exponential) {
	ostrm << exponential.toString();
	return ostrm;
}

Exponential::~Exponential() {
}
