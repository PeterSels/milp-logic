#include <iostream>
#include <math.h>
#include <assert.h>

#include "libAnalyticsDefinitions.h"
#include "Exponential.h"
#include "ExponentialFitter.h"
#include "NewtonRaphsonsMethod.h"
#include "Histogram.h"
#include "DataVectorCorrelator.h"

using namespace std;

double  fA(double x, double a, double b) { 
  return 10.0 + 2 * x; 
};
double dfA(double x, double a, double b) { 
  return 2; 
};

double  fB(double x, double a, double b) { 
  return -1.0 - 2 * x + 3 * x*x; };
double dfB(double x, double a, double b) { 
  return -2 + 6 * x; 
};

double  fC(double x, double a, double b) { 
  return sin(x-1); 
};
double dfC(double x, double a, double b) { 
  return cos(x-1); 
};

int main (int argc, char * const argv[]) {

	if (true) {
		cout << "check: log(e) = " 
      << log(2.71828182845904523536) << endl;
		cout << "so log is indeed the natural logarithm function mathematically called 'ln'" << endl;
	}

	if (true) {
		for (int aSign=+1; aSign<=+1; aSign+=2) {
			// aSign starts at +1 because
			// negative numbers for yi cannot be supported since
			// logarithms of negative numbers are impossible
			cout << ">>> aSign = " << aSign << " <<<" << endl;
			for (int bSign=-1; bSign<=+1; bSign+=2) {
				cout << "  >>> bSign = " << bSign << " <<<" << endl;
				for (double a=aSign*1.0E-3; 
          aSign*a<=1.0E6*(1+TOLERANCE); a*=10.0) {
					cout << "    >>> a = " << a << " <<<" << endl;
					for (double b=bSign*1; bSign*b<=3; b+=bSign) {
						cout << "      >>> b = " << b << " <<<" << endl;
						cout << "          ExponentialFitterTest(a=" 
              << a << ", b=" << b << ")" << endl;		
						Exponential exp0(a, b);
						vector<double> x;
						vector<double> y;
						for (int i=1; i<=5; i++) {
							double xi = i;
							double yi = exp0.eval(i);
							//cout << "(x[" << i << "] = " 
              //<< xi << ", y[" << i << "] = " << yi << ")" << endl;
							x.push_back(xi);
							y.push_back(yi);
						}
						ExponentialFitter exponentialFitter;
						Exponential exp = exponentialFitter.fit(x, y);
						cout << "          Exponential = " << exp << endl;
						assert(exp.getFactor()   <= a*(1+aSign*TOLERANCE));
						assert(exp.getFactor()   >= a*(1-aSign*TOLERANCE));
						assert(exp.getExponent() <= b*(1+bSign*TOLERANCE));
						assert(exp.getExponent() >= b*(1-bSign*TOLERANCE));
					}	
				}
			}
		}
	}
	
	if (true) {
	  cout << "ExponentialTestProperties" << endl;		
		double a = +2.0;
		double b = -1.0;
		int aSign= (int)(a/fabs(a));
		int bSign= (int)(b/fabs(b));
		Exponential exp0(a, b);
		
		cout << "Exponential = " << exp0 << endl;
		assert(exp0.getFactor()   <= a*(1+aSign*TOLERANCE));
		assert(exp0.getFactor()   >= a*(1-aSign*TOLERANCE));
		assert(exp0.getExponent() <= b*(1+bSign*TOLERANCE));
		assert(exp0.getExponent() >= b*(1-bSign*TOLERANCE));
		
		// derived properties
		assert(exp0.valueForXis0() == exp0.getFactor());

		double normalizingFactor = 
      exp0.surfaceUnderCurveForPositiveX();
		assert(normalizingFactor == a/-b);
		assert(exp0.expectedValueForPositiveX() == 1/-b);
		
		double X = a;
		
		double calcXSmallProb = exp0.probabilityThatXIsSmallerThan(X);
		double refXSmallProb = 
      (a/(-b)) * (1-(exp(b*X))) / normalizingFactor;
		cout 
      << "calcXSmallProb = " << calcXSmallProb 
      << ", refXSmallProb = " << refXSmallProb << endl;
		assert(calcXSmallProb == refXSmallProb);
		
		double calcXGreatProb = exp0.probabilityThatXIsGreaterThan(X);
		double refXGreatProb = 
      (a/(-b)) * (exp(b*X)) / normalizingFactor;
		cout 
      << "calcXGreatProb = " << calcXGreatProb 
      << ", refXGreatProb = " << refXGreatProb << endl;
		assert(calcXGreatProb == refXGreatProb);

		// sum of probabilities is 1.0
		assert(calcXSmallProb+calcXGreatProb == 1.0);
		cout << endl;
	}
	/*
	if (true) {
		cout << "ExponentialFitterTest2" << endl;		
		vector<double> x;
		for (int i=0; i<5; i++) {
			x.push_back(i+1);
		}
		vector<double> y;
		y.push_back(0.783);
		y.push_back(0.552);
		y.push_back(0.245);
		y.push_back(0.165);
		y.push_back(0.097);
		
		ExponentialFitter exponentialFitter;
		Exponential exp = exponentialFitter.fit(x, y);
		cout << "Exponential = " << exp << endl;
		assert(exp.getFactor() <= 1.54409 + TOLERANCE);
		assert(exp.getFactor() >= 1.54409 - TOLERANCE);
		assert(exp.getExponent() <= 0.472436 + TOLERANCE);
		assert(exp.getExponent() >= 0.472436 + TOLERANCE);
	}
	*/
  /*
  if (true) {
    NewtonRaphsonsMethod newton;

    double zeroA = newton.findZero(fA, dfA, 100, 0, 0);
    cout << "zero for fA is " << zeroA << endl << endl;

    double zeroB = newton.findZero(fB, dfB, 100, 0, 0);
    cout << "zero for fB is " << zeroB << endl << endl;

    // showing that the outcomingzero depends on the ingoing
    for (int i=-2; i<=+2; i++) {
      double offset = i*3.14;
      double zeroC = newton.findZero(fC, dfC, 1.2+offset, 0, 0);
      cout << "zero for fC is " << zeroC << endl << endl;
    }

    for (double a=1; a<3; a++) {
      for (double b=1; b<8; b++) {
        cout << "exponential(" << a << ", " << b << ")" << endl;
        Exponential exp0(a, b);
        exp0.calcAndSetLambda();
        double lambda = exp0.getLambda();
        cout << "lambda = " << lambda << endl;
        double value = lambda - log(lambda) - b + log(a);
        cout << "fx value = " << value << endl;
        assert(value <= NEWTON_TOLERANCE*10);
      }
    }
  }
  */


  if (true) {
    double avg = 0.0;
    double lambda;
    
    const unsigned int N = 100000;
    Exponential e0(2, -1);
    cout << "e0 = " << e0 << endl;
    for (unsigned int i=0; i<N; i++) {
      double sample = e0.getRandomSample();
      //cout << "sample = " << sample << endl;
      avg += sample;
    }
    avg /= N;
    lambda = e0.getNormalizeLambda();
    cout << "average sample should be close to 1/lambda = 1/" 
      << lambda 
      << " = " << 1/lambda << endl;
    // sanity check:
    assert(avg < 1/lambda * 1.01);
    assert(1/lambda * 0.99 < avg);    
    
    avg = 0.0;
    Exponential e1(2, -3);
    cout << "e1 = " << e1 << endl;
    for (unsigned int i=0; i<N; i++) {
      double sample = e1.getRandomSample();
      //cout << "sample = " << sample << endl;
      avg += sample;
    }
    avg /= N;
    lambda = e1.getNormalizeLambda();
    cout << "average sample should be close to 1/lambda = 1/" 
      << lambda 
    << " = " << 1/lambda << endl;
    // sanity check:
    assert(avg < 1/lambda * 1.01);
    assert(1/lambda * 0.99 < avg);
  }


  if (true) {
    map<unsigned int, unsigned int> distribution;
    distribution[2] = 1;
    distribution[4] = 2;
    distribution[24] = 2;
    distribution[27] = 1;
    distribution[37] = 2;
    distribution[45] = 1;
    distribution[64] = 1;
    distribution[76] = 1;
    ExponentialFitter fitter;
    Exponential exponential = fitter.fit(distribution);
    cout << exponential << endl;
    double expectedValue = 1/exponential.getNormalizeLambda();
    cout << "expectedValue = " << expectedValue << endl;
  }

  
  bool histogram = true;
  
  if (histogram) {
    Histogram h;
    h.addPointXY(0, 1);
    h.addPointXY(2, 1);
    h.addPointXY(4, 1);
    double avg = h.calcAverage();
    assert(avg==2);
    
    ExponentialFitter expFitter;
    Exponential e = expFitter.match(h);
    cout << "exp = " << e << endl;
  }
  
  if (histogram) {
    Histogram h;
    h.incrementOccurrenceOfX(0);
    h.incrementOccurrenceOfX(2);
    h.incrementOccurrenceOfX(4);
    double avg = h.calcAverage();
    assert(avg==2);
    
    ExponentialFitter expFitter;
    Exponential e = expFitter.match(h);
    cout << "exp = " << e << endl;    
  }
  
  if (histogram) {
    for (unsigned o=0; o<10; o++) {
      for (unsigned int i=1; i<10; i++) {
        Histogram h;
        h.addPointXY(1+o, i);
        h.addPointXY(3+o, i);
        h.addPointXY(5+o, i);
        double avg = h.calcAverage();
        assert(avg==3+o);
      }
    }
  }
  
  if (histogram) {
    Histogram h;
    h.inventDistributionWhenAbsent();  
    double avg = h.calcAverage();
    assert(avg == 1/3.0);
    
    ExponentialFitter expFitter;
    Exponential e = expFitter.match(h);
    cout << "exp = " << e << endl;    
  }
  
  if (histogram) {
    Histogram h;
    h.incrementOccurrenceOfX(1);
    h.inventDistributionWhenAbsent();  
    double avg = h.calcAverage();
    assert(avg == 1);
    
    ExponentialFitter expFitter;
    Exponential e = expFitter.match(h);
    cout << "exp = " << e << endl;    
  }
  
  
  if (true) {
    Histogram h(6);
    h.incrementOccurrenceOfX(1);
    h.incrementOccurrenceOfX(2);
    h.incrementOccurrenceOfX(6+1);
    h.incrementOccurrenceOfX(6+2);
    
    int xmin = h.calcXMin();
    cout << "histogram x min = " << xmin << endl;
    int xmax = h.calcXMax();
    cout << "histogram x max = " << xmax << endl;

    double ymax = h.calcYMax();
    cout << "histogram y max = " << ymax << endl;

    double total = h.calcTotal();
    cout << "histogram total = " << total << endl;
    double avg = h.calcAverage();
    cout << "histogram x weighted average = " << avg << endl;
    
    assert(xmin==3);
    assert(xmax==9);
    assert(ymax==2.0);
    assert(avg==6);
  }
    
  // testing linear regression of few points
  if (true) {
    vector<double> x;
    x.push_back(0);
    x.push_back(1);
    vector<double> y;
    y.push_back(1);
    y.push_back(0);
    DataVectorCorrelator dvc(x, y);
    double slope = dvc.getSlope();
    double absis = dvc.getAbsis();
    cout << "slope=" << slope << ", absis=" << absis << endl;
    assert(slope==-1.0);
    assert(absis==1.0);
  }
  
  if (true) {
    vector<double> x;
    x.push_back(1);
    x.push_back(2);
    vector<double> y;
    y.push_back(2);
    y.push_back(3);
    DataVectorCorrelator dvc(x, y);
    double slope = dvc.getSlope();
    double absis = dvc.getAbsis();
    cout << "slope=" << slope << ", absis=" << absis << endl;
    assert(slope==1.0);
    assert(absis==1.0);
  }
  
  
  cout << "done" << endl;

  return 0;
}
