#include <iostream>
#include <assert.h>
#include <limits>

#include "MinimumCalculator.h"
#include "Step.h"

using namespace std;

MinimumCalculator::MinimumCalculator(double (*fPtr)(const std::vector<double>
                                                      & parameters, 
                                                      double d),
                                     const std::vector<double> & parameters,
                                     unsigned int D1,
                                     double * curve,
                                     unsigned int size)
: D1_(D1)
{
  // A more clever (faster) method could be used, but this always works.
  bool verbose = true;
  
  // serial
  {
    double d = 0.0;
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
      d+=STEP;
    } while (d <= D1_); // <= because
    // we want D1_ to be the minimum when function is decreasing
    if (verbose) {
      cerr << "MinimumCalculator:serial: dMin_=" << dMin_ 
           << ", zMin_=" << zMin_ << endl;
      cerr << endl;
    }
    
  }
  
  /*
  // parallel
  {
    double min = numeric_limits<double>::max();
    unsigned int iMin = 0;
//#pragma omp parallel for reduction(+:sum)    
    for (int i=0; i<(int)size; i++) {
      double z = curve[i];
      if (z <= min) {
        min  = z;
        iMin = i;
      }
    }
    if (verbose) {
      cerr << "MinimumCalculator:parallel: dMin_=" << dMin_ 
      << ", zMin_=" << zMin_ << endl;
      cerr << endl;
    }
  }
  */
  
  //assert(zMin_ == (*fPtr)(parameters, dMin_));
  assert(dMin_ <= D1_);
  assert(0 <= dMin_);
  
  assert(zMin_ >= 0);
}

double MinimumCalculator::getMinimumAbsis() const {
  return dMin_;
}

double MinimumCalculator::getMinimumValue() const {
  return zMin_;
}

MinimumCalculator::~MinimumCalculator() {
}
