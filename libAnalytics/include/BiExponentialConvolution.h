#ifndef BI_EXPONENTIAL_CONVOLUTION_H
#define BI_EXPONENTIAL_CONVOLUTION_H

#include <string>

// Exponentials are supposed to be negative and normalized 
// so they both represent: f(x) = exponent_ * e^(-exponent_*x), 
class BiExponentialConvolution {
private:
	double a_;
	double b_;
public:
	BiExponentialConvolution(double aExponent_, double bExponent);
  double getAExponent() const { return a_; };
  double getBExponent() const { return b_; };
	double eval(double x) const;
  double valueForDis0() const;
  
	// derived properties
	double valueForXis0() const;
	double surfaceUnderCurveForPositiveX() const;	
	double expectedValueForPositiveX() const;
	double probabilityThatSumIsLessThan(double D) const;
	double probabilityThatSumIsGreaterThan(double D) const;
	  
  double getRandomSample() const;

	std::string toString() const;
	friend std::ostream & operator << (std::ostream & ostrm, const BiExponentialConvolution & biExpConv);
	~BiExponentialConvolution();
};

#endif // BI_EXPONENTIAL_CONVOLUTION_H
