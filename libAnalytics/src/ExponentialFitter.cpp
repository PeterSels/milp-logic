/*
 *  ExponentialFitter.cpp
 *  Created by Peter Sels on 13/07/09.
 */

#include <iostream>
#include <math.h>
#include <assert.h>

#include "ExponentialFitter.h"
#include "Histogram.h"

using namespace std;

ExponentialFitter::ExponentialFitter() {
}

Exponential ExponentialFitter::match(
  const Histogram & histogram) const {
  
  double avg = histogram.calcAverage();
  if (avg==0.0) {
    avg = 1.0; // avoid division by 0
  }
  double lambda = 1/avg;
  Exponential theExp(1, -lambda);
  return theExp;
}

Exponential ExponentialFitter::fit(
  const vector<double> & x, const vector<double> & y) {
	// See: http://mathworld.wolfram.com/LeastSquaresFittingExponential.hml
	// for the derivation of the formulas for a and b and A and B.

	unsigned int nX = (unsigned int)(x.size());
	unsigned int nY = (unsigned int)(y.size());
	assert(nX==nY);
	unsigned int N = nX;
  double A;
  double B;
  if (N < 2) {
    assert(false);
    assert(N==1);
    A =  1.0;
    B = -1.0;
  } else {
	  for (unsigned int i=0; i<N; i++) {
		  assert(y[i] >= 0.0) ;
	  }	
	  double sumy = 0;
	  double sumxy = 0;
	  double sumxSqy = 0;
	  double sumylny = 0;
	  double sumxylny = 0;
  	
	  for (unsigned int i=0; i<N; i++) {
		  sumy    += y[i];
		  sumxy   += x[i] * y[i];
		  sumxSqy += (x[i] * x[i]) * y[i];
		  sumylny += y[i] * log(y[i]);
		  sumxylny += x[i] * y[i] * log(y[i]);
	  }
	  double aNominator = sumxSqy * sumylny - sumxy * sumxylny;  	
	  double bNominator = sumy * sumxylny - sumxy * sumylny;
	  double denominator = sumy * sumxSqy - sumxy * sumxy;
    if (denominator==0.0) {
      assert(N==1);
      assert(false);
    }
	  double a = aNominator / denominator;
	  double b = bNominator / denominator;  	  	
	  A = exp(a);
	  B = b;
  }
  Exponential theExp(A, B);
	return theExp;
}

Exponential ExponentialFitter::fit(
  const map<unsigned int, unsigned int> & distribution) {

  map<double, double> doubleDistribution;
  for (map<unsigned int, unsigned int>::const_iterator
    it =distribution.begin();
    it!=distribution.end(); it++) {
    double x = (double)(it->first);
    double y = (double)(it->second);
    doubleDistribution[x] = y;
  }
  return fit(doubleDistribution);
}

Exponential ExponentialFitter::fit(
  const map<double, double> & distribution) {
  unsigned int nx = (unsigned int)distribution.size();
  bool verbose = false;
  if (verbose) {
    cout << "NOTE: #Different measurements = " << nx << endl;
  }

  unsigned int nSamples = 0;
  vector<double> xVec;
  vector<double> yVec;
  for (map<double, double>::const_iterator
    it =distribution.begin();
    it!=distribution.end(); it++) {
    double x = (double)(it->first);
    double y = (double)(it->second);
    nSamples += (unsigned int)y;
    xVec.push_back(x);
    yVec.push_back(y);
  }
  if (verbose) {
    cout << "nSamples = " << nSamples << endl;
  }
  // fit histogram of occurences as probability curve
  ExponentialFitter fitter;
  return fitter.fit(xVec, yVec);
}

ExponentialFitter::~ExponentialFitter() {
}
