/*
 *  Exponential.h
 *  ScheduleModelBuilder
 *
 *  Created by Peter Sels on 13/07/09.
 *
 */

#ifndef EXPONENTIAL_H
#define EXPONENTIAL_H

#include <string>

// Exponential represents: f(x) = factor_ * e^(exponent_*x), 
// factor_ and exponent_ can be positive or negative
class Exponential {
private:
	double factor_;
	double exponent_;
  double normalizeLambda_;
  double normalizeFactor_;
public:
	Exponential(double factor, double exponent);
  void init(double factor, double exponent);
	inline double getFactor() const { return factor_; };
	double getExponent() const { return exponent_; };
  double getNormalizeLambda() const { return normalizeLambda_; };
	double eval(double x) const;

	// derived properties
  void setNormalizeLambda();
  void setNormalizeFactor();
	double valueForXis0() const;
	double surfaceUnderCurveForPositiveX() const;	
	double expectedValueForPositiveX() const;
	double probabilityThatXIsSmallerThan(double X) const;
	double probabilityThatXIsGreaterThan(double X) const;
	  
  //double function(double x) /*const*/;
  //double derivative(double x) /*const*/;
  double getRandomSample() const;


	std::string toString() const;
	friend std::ostream & operator << (std::ostream & ostrm, const Exponential & exponential);
	~Exponential();
};

//double lambdaFunction(double x, double a, double b);
//double lambdaDerivative(double x, double a, double b);

#endif // EXPONENTIAL_H
