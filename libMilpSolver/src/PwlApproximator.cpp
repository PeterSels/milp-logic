#include <assert.h>
#include <iostream>

#include "PwlApproximator.h"
#include "MinimumCalculator.h"
#include "BreakPointCalculator.h"
#include "DataVectorCorrelator.h"
#include "Step.h"

#include "DoOpenMP.h"
#ifdef DO_OPEN_MP
#include <omp.h>
#endif
#include "MaxThreads.h"

#define TOLERANCE (1E-2)

using namespace std;

PwlApproximator::PwlApproximator(bool brkPointNotMinimum) 
: z0_(0.0)
, D1_(0)
, zD1_(0.0)

, dMin_(0)
, zMin_(0.0)

, brkPointNotMinimum_(brkPointNotMinimum)
{
}

PwlApproximator::PwlApproximator(bool brkPointNotMinimum,
                                 double (*fPtr)(const std::vector<double>
                                                & parameters, 
                                                double d),
                                 const std::vector<double> & parameters,
                                 unsigned int D1,
                                 unsigned int dBrk) 
/// calc min & interpolate

{
  const int SIZE = (int)(D1 / STEP) + 1;
  double curve[SIZE];
  int iMin = 0;
  double zMin = numeric_limits<double>::max();
//#pragma omp parallel for num_threads(MAX_N_THREADS)
  for (int i=0; i<SIZE; i++) {
    double d = i * STEP;
    double z = (*fPtr)(parameters, d); // only here, should call fPtr
    curve[i] = z;
    // While you are at it, can calculate real minimum
    // only to be overwritten in case of breakPoint io minPoint needed.
    // See below: BreakPointCalculator.
    if (z <= zMin) { 
      zMin  = z;
      iMin = i;
    }
  }
  assert(iMin >= 0);
  assert(iMin < SIZE);
  assert(zMin >=0);
  
  int dMinTemp = iMin * STEP;
  double zMinTemp = zMin;
  
  brkPointNotMinimum_ = brkPointNotMinimum;
/*
  bool fNotIncreasing = true;
  bool fNotDecreasing = true;

#pragma omp parallel sections
  {
    
#pragma omp section
    {
      // only decides fNotIncreasing
      //zPrev = (*fPtr)(parameters, 0);
      double zPrev = curve[0];
      double d = 0;
      //for (double d=0; (d<=D1) && fNotIncreasing; d+=STEP) {
      for (int i=0; i<SIZE && fNotIncreasing; i++, d+= STEP) {
        //double z = (*fPtr)(parameters, d);
        double z = curve[i];
        if (z > zPrev) {
          fNotIncreasing = false;
        }
        zPrev = z;
      }
    }
  
#pragma omp section
    {
      // only decides fNotDecreasing
      //zPrev = (*fPtr)(parameters, 0);
      double zPrev = curve[0];
      double d = 0;
      //for (double d=0; (d<=D1) && fNotDecreasing; d+=STEP) {
      for (int i=0; (i<SIZE) && fNotDecreasing; i++, d+=STEP) {
        //double z = (*fPtr)(parameters, d);
        double z = curve[i];
        if (z < zPrev) {
          fNotDecreasing = false;
        }
        zPrev = z;
      }
    }
    
  } // end parallel sections
*/
  
  // left point
  // 0
  //z0_   = (*fPtr)(parameters, 0);
  z0_   = curve[0];
  
  // right point
  D1_  = D1;
  //zD1_ = (*fPtr)(parameters, (int)D1_);
  //zD1_ = (*fPtr)(parameters, D1_);
  zD1_ = curve[SIZE-1];
  assert(zD1_ >= 0);
  
  // middle (low, minimal) point
  if (brkPointNotMinimum_) {
    //BreakPointCalculator brkCalc(fPtr, parameters, D1, fNotIncreasing, dBrk);
    BreakPointCalculator brkCalc(curve, SIZE, D1, dBrk, STEP);
    dMin_ = brkCalc.getBreakPointAbsis();
    zMin_ = brkCalc.getBreakPointValue();
  } else {
    /*
    MinimumCalculator minCalc(fPtr, parameters, D1, curve, SIZE);
    dMin_ = minCalc.getMinimumAbsis();
    zMin_ = minCalc.getMinimumValue(); 
    assert(dMinTemp == dMin_);
    assert(zMinTemp == zMin_);
     */
    dMin_ = dMinTemp;
    zMin_ = zMinTemp;
  }

  assert(0 <= dMin_);
  assert(dMin_ <= D1_);
  
  const bool linearRegression = true;
  if (linearRegression) {
    const unsigned int MIN_POINTS_FOR_REGRESSION = 4;
    
    double slopeLeft;
    double absisLeft;
    vector<double> xLeft;
    vector<double> yLeft;
    
    const double SMALLER_STEP = STEP/1; // changed from 4 to 1, CHECKMEEEEEEEEEEEEEEEEE
    // should work better than STEP for dwell costs, 
    // where there are few samples in STEP case,
    // what does it do for transfer costs? // FIXME
    
    unsigned int nPointsLeft = (dMin_ - 0.0)/SMALLER_STEP + 1;
    if (nPointsLeft >= MIN_POINTS_FOR_REGRESSION) {
      /*
      for (double d=0; d<=dMin_; d+=SMALLER_STEP) {
        //double z = (*fPtr)(parameters, d);
        double z = curve[i];
        xLeft.push_back(d);
        yLeft.push_back(z);
      }
      DataVectorCorrelator dvcLeft(xLeft, yLeft);
      */
      
      DataVectorCorrelator dvcLeft(0, nPointsLeft, STEP, curve);
      
      slopeLeft = dvcLeft.getSlope();
      absisLeft = dvcLeft.getAbsis();
    } else { // not enough data for regression
      assert(dMin_>=0);
      slopeLeft = 0.0;
      absisLeft = zMin_;
    }

    
    double slopeRight;
    double absisRight;
    vector<double> xRight;
    vector<double> yRight;
    unsigned int nPointsRight = (D1_ - dMin_)/STEP + 1;
    if (nPointsRight >= MIN_POINTS_FOR_REGRESSION) {
      /*
      for (double d=dMin_//+STEP
        ; d<=D1_; d+=STEP) {
        double z = (*fPtr)(parameters, d);
        xRight.push_back(d);
        yRight.push_back(z);
      }
      DataVectorCorrelator dvcRight(xRight, yRight);
      */
      unsigned int rightPointIndex = dMin_ / STEP;
      DataVectorCorrelator dvcRight(rightPointIndex, nPointsRight, STEP, curve);
      slopeRight = dvcRight.getSlope();
      absisRight = dvcRight.getAbsis();
    } else { // not enough data for regression
      assert(dMin_>=D1_*(1-TOLERANCE));
      assert(dMin_<=D1_*(1+TOLERANCE));
      slopeRight = 0.0;
      absisRight = zMin_;      
    }
    
    // (Slightly) adapt (0, z0_), (D1_, zD1_) and then,
    // their intersection: (dMin_, zMin_):

    // 0 remains 0 of course
    z0_ = absisLeft + (0 * slopeLeft);
    assert(z0_ >= 0);
    
    // D1_ remains D1_ of course
    zD1_ = absisRight + (D1_ * slopeRight); // DEBUG, FIXME
    if (zD1_ < 0) {
      const bool verbose = false;
      if (verbose) {
        cerr << "NOTE: Allowing LinReg caused: zD1_=" << zD1_ << "<0" << endl;
      }
      /*
      cerr << "WARNING: Correcting zD1_ from " << zD1_ 
      << " to " << 0 << endl;
      zD1_ = 0;
      assert(false);
      */
    }
    if (!linearRegression) {
      assert(zD1_ >= 0);
    }
    
    if (slopeLeft == slopeRight) { // all flatliners
      assert(slopeLeft==0);
      assert(absisLeft==0);
      assert(absisRight==0);
      dMin_ = 0;
      zMin_ = 0;
    } else {
      //cerr << "dMin_ = " << dMin_ << " zMin_ = " << zMin_ << endl;
      crossingLinesIntersect(dMin_, zMin_, 
                             absisLeft, slopeLeft, 
                             absisRight, slopeRight);
      if (dMin_ < 0) { // for a SOURCE edge
        cerr << "WARNING: Correcting dMin_ from " << dMin_ 
        << " to " << 0 << endl;
        dMin_ = 0;
        assert(0 <= dMin_);
        assert(false);
      }
      if (dMin_ > D1_) {
        cerr << "WARNING: Correcting dMin_ from " << dMin_ 
        << " to " << D1_ << endl;
        dMin_ = D1;
        assert(false);
      }
      assert(dMin_ <= D1_);
    }
  } // end of linearRegression
  
  if (dMin_ < 0) {
    assert(0 <= dMin_);
  }
  assert(0 <= dMin_);
  assert(dMin_ <= D1_);
}

void PwlApproximator::crossingLinesIntersect(double & x, double & y,
                                             double absis0, double slope0,
                                             double absis1, double slope1) {
  // from http://mathworld.wolfram.com/Line-LineIntersection.html
  
  // (x1,y1) and (x2, y2) are points on this left line:
  double x1 = 0;
  double y1 = z0_;
  double x2 = D1_;
  double y2 = absis0 + D1_ * slope0;
  
  // (x3,y3) and (x4, y4) are points on right line
  double x3 = 0;
  double y3 = absis1 + (0 * slope1);
  double x4 = D1_;
  double y4 = zD1_;
  
  // the commented out part could be more numerically unstable
  double x12 = x1-x2;
  double x34 = x3-x4;
  double y12 = y1-y2;
  double y34 = y3-y4;
  
  double denominator = x12 * y34 - y12 * x34;
  if (denominator == 0) { // lines are parallel
    cerr << "ERROR: Point2D Line2D::intersect(Line2D aLine2D) const" << endl
    << "  No intersection." << endl;
    assert(denominator!=0);
    exit(0); // invalid point
  }
  double nominatorLU = x1*y2 - y1*x2;
  double nominatorLD = x3*y4 - y3*x4;  
  x = (nominatorLU * x34 - nominatorLD * x12)/denominator;
  y = (nominatorLU * y34 - nominatorLD * y12)/denominator;
}

bool PwlApproximator::getBrkPointNotMinimum() const {
  return brkPointNotMinimum_;
}

void PwlApproximator::setBrkPointNotMinimum(bool brkPointNotMinimum) {
  brkPointNotMinimum_ = brkPointNotMinimum;
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
