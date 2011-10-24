#include <iostream>
#include <assert.h>

#include "BreakPointCalculator.h"

using namespace std;

BreakPointCalculator::BreakPointCalculator(double (*fPtr)(const std::vector<double>
                                                          & parameters, 
                                                          int ii),
                                           const std::vector<double> & parameters,
                                           unsigned int D1,
                                           bool fDecreasing) 
: D1_(D1)
{
  // A more clever (faster) method could be used, but this always works.
  bool verbose = false;
  bool enoughLowered = true;  
  
  if (fDecreasing) {
    int d = 0;
    dBrk_ = d;
    zBrk_ = (*fPtr)(parameters, d);
    //const double SLOPE_DOWN_THRESHOLD = -0.1; // absolute, not so useful
    const double SLOPE_DOWN_THRESHOLD = -0.001 * zBrk_; // relative is better
    
    double prevZ = zBrk_; // initial value not used anyway
    do {
      double z = (*fPtr)(parameters, d);
      //if (z <= zBrk_) {
      if ((d==0) || ((z - prevZ < SLOPE_DOWN_THRESHOLD))) {
        zBrk_ = z;
        dBrk_ = d;
        enoughLowered = true;
      } else {
        enoughLowered = false;
      }
      if (verbose) {
        cout << "d=" << d << ", z=" << z;
        if (enoughLowered) {
          cout << " <- dn";
        }
        cout << endl;
      }
      d++;
      prevZ = z;
    } while (d <= D1_); // <= because
    // we want D1_ to be the BreakPoint when function is decreasing
  } else {
    int d = D1;
    dBrk_ = d;
    zBrk_ = (*fPtr)(parameters, d);
    //const double SLOPE_DOWN_THRESHOLD = -0.1; // absolute, not so useful
    const double SLOPE_DOWN_THRESHOLD = -0.001 * zBrk_; // relative is better
    
    double prevZ = zBrk_; // initial value not used anyway
    do {
      double z = (*fPtr)(parameters, d);
      //if (z <= zBrk_) {
      if ((d==(int)D1) || ((z - prevZ < SLOPE_DOWN_THRESHOLD))) {
        zBrk_ = z;
        dBrk_ = d;
        enoughLowered = true;
      } else {
        enoughLowered = false;
      }
      if (verbose) {
        cout << "d=" << d << ", z=" << z;
        if (enoughLowered) {
          cout << " <- dn";
        }
        cout << endl;
      }
      d--;
      prevZ = z;
    } while (d >= 0); // <= because
    // we want 0 to be the BreakPoint when function is increasing    
  }
  
  if (verbose) {
    cerr << "BreakPointCalculator: dBrk_=" << dBrk_ << ", zBrk_=" << zBrk_ 
    << endl;
    cerr << endl;
  }
  //assert(zBrk_ == (*fPtr)(parameters, dBrk_));
}

double BreakPointCalculator::getBreakPointAbsis() const {
  return dBrk_;
}

double BreakPointCalculator::getBreakPointValue() const {
  return zBrk_;
}

BreakPointCalculator::~BreakPointCalculator() {
}
