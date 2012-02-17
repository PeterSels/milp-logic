#include <assert.h>
#include <iostream>
#include <math.h>

#include "PwlApproximator.h"
#include "MinimumCalculator.h"
#include "BreakPointCalculator.h"
#include "DataVectorCorrelator.h"
#include "Step.h"
#include "NewtonRaphsonsMethod.h"

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

// Finding minimum of f(x) by finding roots of derivative df(x)
// by using Newton Raphson method. Fast, but no convergence guarantee.
unsigned int PwlApproximator
::findMinimumIndexUsingNewtonRaphson(unsigned int iGuess, 
                                     unsigned int iLo, 
                                     unsigned int iHi,
                                     double * yValues) const {
  assert(iLo <= iGuess);
  assert(iGuess <= iHi);
  
  const unsigned int MAX_N_STEPS = 80;
  
  cout << "findMinimumIndex" << endl;
  
  double x  = iGuess;
  double xAbsTolerance = 1.0;
  double error;
  unsigned int nSteps = 0;
  double prevX = numeric_limits<double>::max();
  do {
    int intx = (int)(x+0.5); // rounding
    assert(intx >= 0);
    
    double fx;    
    double dfx;
    double ddfx;
    calcDerivatives(fx, dfx, ddfx, 
                    intx, iLo, iHi, yValues);
    
    cout << "  " << nSteps << ": x=" << x << ", fx=" << fx << ", dfx=" << dfx 
    << ", ddfx=" << ddfx << endl;
    
    if (dfx==0) { // done
      return intx;
    } else {
      cout << "decrement (dfx/ddfx) = " << (dfx/ddfx) << endl;
      x -= dfx/ddfx;
      cout << "    x=" << x << endl;
      // since we can assume a convex function,
      // leaving the interval once will never bring us back
      if (x < iLo) {
        return iLo;
      } else if (x > iHi) {
        return iHi;
      }      
      nSteps++;
      error = ::fabs(x-prevX);
      prevX = x;
    }
  } while ((error > xAbsTolerance) && 
           (nSteps < MAX_N_STEPS)); // avoid infinite loop
  
  assert(error < 1); // otherwise, takes too many steps somehow
  
  cout << "nSteps = " << nSteps;
  return x;
}

void PwlApproximator::calcDerivatives(double & fx,
                                      double & dfx, 
                                      double & ddfx,
                                      unsigned int intx,
                                      unsigned int iLo, unsigned int iHi,
                                      double * yValues) const {
  fx = yValues[intx];
  if (intx+1 <= iHi) { // fwd derivative
    dfx = (yValues[intx+1] - yValues[intx]); // divided by 1 (for 1 i increment)
    double fwd_dfx;
    if (intx+2 <= iHi)  {
      fwd_dfx = (yValues[intx+2] - yValues[intx+1]); // divided by 1 (for 1 i increment)
    } else {
      fwd_dfx = dfx; // assume fwd aextrapolation
    }
    ddfx = fwd_dfx - dfx;
  } else { // bwd derivative
    assert(intx+1 > iHi);
    if (intx <= iHi) {
      assert(iLo <= intx-1);
      double bwd_dfx = (yValues[intx] - yValues[intx-1]); // divided by 1 (for 1 i increment)
      dfx = bwd_dfx; // ssume bwd extrapolation
      assert(iLo <= intx-1-1);
      double bwd_bwd_dfx = (yValues[intx-1] - yValues[intx-2]); // divided by 1 (for 1 i increment)
      ddfx = (bwd_dfx - bwd_bwd_dfx);
    } else {
      assert(false);
    }
  }  
}

// Since the Newton Raphson version does not converge in many cases,
// we use a method that does.
// Regula Falsi: we want to find the minimum of the function with
// values stored in yValues. For this, we user the Regula Falsi method
// to find the root of the derivative of this function.
// For effeciency reasons, the derivative is calculated only in the points 
// visited by the Regula Falsi method.
// Regula Falsi is guaranteed to converge, but converges slower
// than Newton Raphson (when this one does)
// but still faster than the bisection method.
// See: http://www.cs.purdue.edu/homes/enh/courses/cs158a/cs158ap1/c8.pdf
unsigned int PwlApproximator
::findMinimumIndexUsingRegulaFalsi(bool skipFirstIncreasingFxPart,
                                   unsigned int iLo, 
                                   unsigned int iHi,
                                   double * yValues) const {
  assert(iLo <= iHi);
  
  const unsigned int MAX_N_STEPS = 100;
  
  assert(iLo==0);
  double iLoCurr = (double)iLo;
  double dfxLo;
  // Application Specific for transfer curve.
  // Skip samples where fx is going up with increasing x
  bool increasing;
  if (skipFirstIncreasingFxPart) {
    do {  
      double fxLo, dfxLo, ddfxLo;
      calcDerivatives(fxLo, dfxLo, ddfxLo,
                      iLoCurr, iLo, iHi, yValues);
      //cout << "  iLo=" << iLo << ", fxLo=" << yValues[iLo] 
      // << ", dfxLo=" << dfxLo 
      // << ", ddfxLo=" << ddfxLo << endl;
      increasing = (dfxLo >= 0);
      if (increasing) {
        iLoCurr++;
      }
    } while (increasing); // skip first root, which is a maximum for fx
  }
  
  // Regula Falsi
  double x; //  = iGuess;
  double xAbsTolerance = 0.1;
  double error;
  unsigned int nSteps = 0;
  double iHiCurr = (double)iHi;
  double prevX = numeric_limits<double>::max();
  do {
    
    //cout << "  " << nSteps << ":" << endl;
    double fxLo, dfxLo, ddfxLo;
    calcDerivatives(fxLo, dfxLo, ddfxLo,
                    iLoCurr, iLo, iHi, yValues);
    //cout << "  iLo=" << iLo << ", fxLo=" << yValues[iLo] << ", dfxLo=" << dfxLo 
    //<< ", ddfxLo=" << ddfxLo << endl;
    
    
    double fxHi, dfxHi, ddfxHi;
    calcDerivatives(fxHi, dfxHi, ddfxHi,
                    iHiCurr, iLo, iHi, yValues);
    //cout << "  iHi=" << iHi << ", fxHi=" << yValues[iHi] << ", dfxHi=" << dfxHi 
    //<< ", ddfxHi=" << ddfxHi << endl;
    
    assert(dfxLo * dfxHi <= 0); // opposite sign or one or both are 0
    
    double denominator = dfxHi-dfxLo;
    if (denominator==0) { // done
      assert(dfxLo==0);
      assert(dfxHi==0);
      // both iLoCurr and iHiCurr are now roots
      return iLoCurr;
    } else {
      double numerator = dfxHi * iLoCurr - dfxLo * iHiCurr;
      x = numerator/denominator;
      double fx, dfx, ddfx;
      calcDerivatives(fx, dfx, ddfx,
                      x, iLo, iHi, yValues);
      if (dfx * dfxLo >= 0) { // same sign, update iLo, increase it
        assert(iLoCurr < x);
        iLoCurr = x;
      } else { // same sign, update iHi
        assert(dfx * dfxHi >= 0);
        assert(x < iHiCurr);
        iHiCurr = x;
      }
      
      //cout << "    x=" << x << endl;
      // since we can assume a convex function,
      // leaving the interval once will never bring us back
      if (x < iLo) {
        return iLo;
      } else if (x > iHi) {
        return iHi;
      }      
      nSteps++;
      //error = ::fabs(iHiCurr - iLoCurr);
      error = ::fabs(x - prevX);
      //error = ::fabs(x - prevX);
      prevX = x;
    }
  } while (error >= xAbsTolerance); // && 
  //(nSteps < MAX_N_STEPS)); // avoid infinite loop
  
  assert(error < xAbsTolerance); // otherwise, takes too many steps somehow
  
  //cout << "nSteps = " << nSteps;
  return x;
}

PwlApproximator::PwlApproximator(bool brkPointNotMinimum,
                                 double (*fPtr)(const std::vector<double>
                                                & parameters, 
                                                double d),
                                 const std::vector<double> & parameters,
                                 unsigned int D1,
                                 unsigned int dBrk) 
/// calc min & interpolate
: D1_(D1)
{
  
  brkPointNotMinimum_ = brkPointNotMinimum;

  // calculate curve array:
  const int SIZE = (int)(D1 / STEP) + 1;
  double curve[SIZE];
  
#ifdef DO_OPEN_MP  
  omp_set_dynamic(1);
#endif
  //num_threads(MAX_N_THREADS)
#pragma omp parallel for if (SIZE > 1000) 
  for (int i=0; i<SIZE; i++) {
    double d = i * STEP;
    double z = (*fPtr)(parameters, d); // only here, should call fPtr
    curve[i] = z;
  }
#ifdef DO_OPEN_MP  
  omp_set_dynamic(0);
#endif  
  
  if (!brkPointNotMinimum_) {

    // left point
    z0_   = curve[0];
    
    // right point
    zD1_ = curve[SIZE-1];
    
    // middle low point
    // serial:
    // While you are at it, can calculate real minimum
    // only to be overwritten in case of breakPoint io minPoint needed.
    // See below: BreakPointCalculator.
    int iMin = 0;
    double zMin = numeric_limits<double>::max();
    if (!brkPointNotMinimum_) {
      for (int i=0; i<SIZE; i++) {
        double z = curve[i];
        if (z < zMin) {
          zMin  = z;
          iMin = i;
          //cout << "new zMin = " << zMin << " for iMin = " << iMin << endl;
        }
      }
      assert(iMin >= 0);
      assert(iMin < SIZE);
      assert(zMin >=0);
    }
    dMin_ = iMin * STEP;
    zMin_ = zMin;

    /* used to be: but maybe we want to keep curve[] data here later...
     MinimumCalculator minCalc(fPtr, parameters, D1, curve, SIZE);
     dMin_ = minCalc.getMinimumAbsis();
     zMin_ = minCalc.getMinimumValue(); 
     */

    /* not converging or too slowly so not worth it, better choose a parallel
       method 
    // try to replace this minimum calculation with faster Newton Raphson
    // But NR is unstable for non convex functions. For transfers, 
    // on the left side, we have a slight inconvexity due to convolution.
    //unsigned int iGuess = (int)(dBrk / STEP);
    //cout << "iGuess = " << iGuess << endl;
    const unsigned int iLo = 0;
    const unsigned int iHi = SIZE-1;
    //unsigned int iMinAgain = 
    //findMinimumIndexUsingNewtonRaphson(iGuess, iLo, iHi, curve);
    unsigned int iMinAgain = 
    findMinimumIndexUsingRegulaFalsi(false, iLo, iHi, curve);
    if (!isEqual(iMin, iMinAgain, 1.0)) { // must be same integer
      cout << "iMin=" << iMin << " != iMinAgain=" << iMinAgain << endl;
    }
    */
    
  } else {
    assert(brkPointNotMinimum_);
    // left point
    z0_   = (*fPtr)(parameters, 0);
    
    // right point
    zD1_ = (*fPtr)(parameters, D1_);
    
    // middle low point
    BreakPointCalculator brkCalc(curve, SIZE, D1, dBrk, STEP);
    dMin_ = brkCalc.getBreakPointAbsis();
    zMin_ = brkCalc.getBreakPointValue();
  }
  
  assert(zD1_ >= 0);
    
  assert(0 <= dMin_);
  assert(dMin_ <= D1_);
  
  const bool linearRegression = true;
  if (linearRegression) {
    const unsigned int MIN_POINTS_FOR_REGRESSION = 4;
    
    double slopeLeft;
    double absisLeft;
    //vector<double> xLeft;
    //vector<double> yLeft;
    
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
