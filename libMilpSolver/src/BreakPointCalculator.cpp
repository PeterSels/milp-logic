#include <iostream>
#include <assert.h>

#include "BreakPointCalculator.h"
#include "Step.h"
#include "libAnalyticsDefinitions.h"
  // for TOLERANCE

using namespace std;

BreakPointCalculator::BreakPointCalculator(double (*fPtr)
                                           (const std::vector<double>
                                            & parameters, 
                                            double d),
                                           const std::vector<double> & 
                                           parameters,
                                           unsigned int D1,
                                           bool fDecreasing) 
: D1_(D1)
{
  // A more clever (faster) method could be used, but this always works.
  bool verbose = false;
  bool enoughLowered = true;  
  
  //const double SLOPE_DOWN_THRESHOLD = 0;
  const double SLOPE_DOWN_THRESHOLD = -1.0 * STEP; // relative is better
  // This corresponds to 45 degrees down point.
  // This may not look like it in the graphics, when x and y axis are on a
  // different scale though.
  
  if (fDecreasing) {
    double d = 0;
    dBrk_ = d;
    zBrk_ = (*fPtr)(parameters, d);
    //const double SLOPE_DOWN_THRESHOLD = -0.1; // absolute, not so useful
    //const double SLOPE_DOWN_THRESHOLD = -0.001 * zBrk_ * STEP; // relative is better
    
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
      d+=STEP;
      prevZ = z;
    } while (d <= D1_); // <= because
    // we want D1_ to be the BreakPoint when function is decreasing
  } else {
    double d = D1;
    dBrk_ = d;
    zBrk_ = (*fPtr)(parameters, d);
    //const double SLOPE_DOWN_THRESHOLD = -0.1; // absolute, not so useful
    //const double SLOPE_DOWN_THRESHOLD = -0.001 * zBrk_ * STEP; // relative is better
    
    double prevZ = zBrk_; // initial value not used anyway
    do {
      double z = (*fPtr)(parameters, d);
      //if (z <= zBrk_) {
      if (((int)d==(int)D1) || ((z - prevZ < SLOPE_DOWN_THRESHOLD))) {
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
      d-=STEP;
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
  
  if (zBrk_ < 0) {
    assert(zBrk_ > -TOLERANCE);
    cerr << "WARNING: Changing zBrk_ from " << zBrk_ << " to " << 0.0 << endl;
    zBrk_ = 0;
  }
}

double BreakPointCalculator::getBreakPointAbsis() const {
  return dBrk_;
}

double BreakPointCalculator::getBreakPointValue() const {
  return zBrk_;
}

BreakPointCalculator::~BreakPointCalculator() {
}
