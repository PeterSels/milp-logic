#include <iostream>
#include <assert.h>

#include "MinimumCalculator.h"

using namespace std;

MinimumCalculator::MinimumCalculator(double (*fPtr)(const std::vector<double>
                                                      & parameters, 
                                                      int ii),
                                     const std::vector<double> & parameters,
                                     unsigned int D1)
: D1_(D1)
{
  // A more clever (faster) method could be used, but this always works.
  bool verbose = false;
  
  unsigned int d = 0;
  dMin_ = d;
  zMin_ = (*fPtr)(parameters, d);
  bool lowered = true;
  do {
    double z = (*fPtr)(parameters, d);
    if (z <= zMin_) {
      zMin_ = z;
      dMin_ = d;
      lowered = true;
    } else {
      lowered = false;
    }
    if (verbose) {
      cout << "d=" << d << ", z=" << z;
      if (lowered) {
        cout << " <- min";
      }
      cout << endl;
    }
    d++;
  } while (d <= D1_); // <= because
  // we want D1_ to be the minimum when function is decreasing
  if (verbose) {
    cerr << "MinimumCalculator: dMin_=" << dMin_ << ", zMin_=" << zMin_ << endl;
    cerr << endl;
  }
  //assert(zMin_ == (*fPtr)(parameters, dMin_));
}

double MinimumCalculator::getMinimumAbsis() const {
  return dMin_;
}

double MinimumCalculator::getMinimumValue() const {
  return zMin_;
}

MinimumCalculator::~MinimumCalculator() {
}
