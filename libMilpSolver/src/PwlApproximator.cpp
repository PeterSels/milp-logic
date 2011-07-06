#include <assert.h>
#include <iostream>

#include "PwlApproximator.h"
#include "MinimumCalculator.h"

PwlApproximator::PwlApproximator() {
  z0_   = 0.0;
  
  D1_   = 0;
  zD1_  = 0.0;
  
  dMin_ = 0;
  zMin_ = 0.0;
}

PwlApproximator::PwlApproximator(double (*fPtr)(const std::vector<double>
                                                & parameters, 
                                                int ii),
                                 const std::vector<double> & parameters,
                                 unsigned int D1) 
/// calc min & interpolate

{
  // left point
  // 0
  z0_   = (*fPtr)(parameters, 0);

  // right point
  D1_  = D1;
  zD1_ = (*fPtr)(parameters, (int)D1_);

  // middle (low, minimal) point
  MinimumCalculator minCalc(fPtr, parameters, D1);
  dMin_ = minCalc.getMinimumAbsis();
  zMin_ = minCalc.getMinimumValue();
}

double PwlApproximator::eval(double d) const {
  assert(0 <=d);
  if (d > D1_) {
    //std::cerr << "";
    //assert(d <= D1_);
  }
  
  double z;
  if (d==dMin_) {
    z = zMin_;
  } else if (d < dMin_) {
    assert(dMin_ > 0);
    z = z0_ + (zMin_ - z0_)/(dMin_ - 0) * (d - 0); 
  } else {
    assert(d >= dMin_);
    if (D1_ > dMin_) {
      z = zMin_ + (zD1_ - zMin_)/(D1_ - dMin_) * (d - dMin_); 
    } else {
      z = zMin_;
    }
  }
  return z;
}

double PwlApproximator::getD(unsigned int i) const {
  assert(0<=i);
  assert(i<=2); // for now
  if (i==0) {
    return 0;
  } else if (i==1) {
    return dMin_;
  } else {
    return D1_;
  }
}

double PwlApproximator::getZ(unsigned int i) const {
  assert(0<=i);
  assert(i<=2); // for now
  if (i==0) {
    return z0_;
  } else if (i==1) {
    return zMin_;
  } else {
    return zD1_;
  }
}

PwlApproximator::~PwlApproximator() {
}
