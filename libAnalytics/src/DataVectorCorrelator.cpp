#include <math.h>
#include <assert.h>
#include <iostream>

#include "DataVectorCorrelator.h"

// using numerically stable Covariance calculation method from Wikipedia
// http://en.wikipedia.org/wiki/Correlation

// From the MatLab help index:
// cov(X) or cov(X,Y) normalizes by N-1 where N is the number of observations. 
// This makes cov(X) the best unbiased estimate of the covariance matrix 
// if the observations are from a normal distribution.

// We currently normalize with N io N-1.
// http://en.wikipedia.org/wiki/Linear_regression and
// http://en.wikipedia.org/wiki/Correlation 
// mention N-1.
// Wolfram at 
// http://mathworld.wolfram.com/LeastSquaresFitting.html
// mentions N. This is a simplification. OK for large N.

using namespace std;

DataVectorCorrelator::DataVectorCorrelator(const std::vector<double> & v0,
                                           const std::vector<double> & v1) {
  
  unsigned int size0 = v0.size();
  unsigned int size1 = v1.size();
  assert(size0==size1);
  unsigned int size = size0;
  assert(size >= 2);
  
  for (unsigned int i=0; i<2; i++) {
    mean_[i]              = 0;
    variance_[i]          = 0;
    standardDeviation_[i] = 0;
  }
  covariance_  = 0;
  correlation_ = 0;

  if (size!=0) {
    mean_[0] = v0[0];
    mean_[1] = v1[0];
    double sumSquares0 = 0;
    double sumSquares1 = 0;
    double sumCoProduct01 = 0;
    unsigned int prevI = 1;
    for (unsigned int i=2; i<=size; i++) {
      double sweep = prevI/(double)(i);
      double delta0 = v0[prevI] - mean_[0];
      double delta1 = v1[prevI] - mean_[1];
      sumSquares0    += delta0 * delta0 * sweep;
      sumSquares1    += delta1 * delta1 * sweep;
      sumCoProduct01 += delta0 * delta1 * sweep;
      mean_[0] += delta0/i;
      mean_[1] += delta1/i;
      prevI = i;
    }
    // 0
    variance_[0]          = sumSquares0/size;
    standardDeviation_[0] = sqrt(variance_[0]);
    // 1
    variance_[1]          = sumSquares1/size;
    standardDeviation_[1] = sqrt(variance_[1]);
    // 01
    covariance_           = sumCoProduct01/size;
    correlation_          = covariance_ / 
      (standardDeviation_[0] * standardDeviation_[1]);
    
    // check: DEBUG
    /*
    double meanx = 0.0;
    double meany = 0.0;
    for (unsigned int i=0; i<size; i++) {
      meanx += v0[i];
      meany += v1[i];
    }
    meanx /= size;
    meany /= size;
    
    cerr << "meanx = " << meanx << ", mean_[0]=" << mean_[0] << endl;
    cerr << "meany = " << meany << ", mean_[1]=" << mean_[1] << endl;
    // end debug
    */
    
    // linear regression of 1 to 0 ( = y to x)
    b_ = covariance_ / variance_[0];
    a_ = mean_[1] - b_ * mean_[0];
  }
}

double DataVectorCorrelator::getMean(unsigned int i) const {
  return mean_[i];
}

double DataVectorCorrelator::getStandardDeviation(unsigned int i) const {
  //  = sigma = sqrt(variance)
  return standardDeviation_[i];
}

double DataVectorCorrelator::getVariance(unsigned int i) const {
  return variance_[i];
}

double DataVectorCorrelator::getCovariance() const {
  return covariance_;
}

double DataVectorCorrelator::getCorrelation() const {
  return correlation_;
}

double DataVectorCorrelator::getSlope() const {
  return b_;
}

double DataVectorCorrelator::getAbsis() const {
  return a_;
}

const std::string DataVectorCorrelator::getName() const {
  return "DataVectorCorrelator";
}

bool DataVectorCorrelator::txtWriteMembers(std::ostream & txtOStream) const {
  bool result = true;
  for (unsigned int i=0; i<2; i++) {
    txtOStream << "  mean_             [" << i << "] = " 
      << mean_[i] << std::endl;
    txtOStream << "  variance_         [" << i << "] = " 
      << variance_[i] << std::endl;
    txtOStream << "  standardDeviation_[" << i << "] = " 
      << standardDeviation_[i] << std::endl;
  }
  txtOStream << "  covariance_           = " << covariance_ << std::endl;
  txtOStream << "  correlation_          = " << correlation_ << std::endl;  
  return result;
}

DataVectorCorrelator::~DataVectorCorrelator() {
}

