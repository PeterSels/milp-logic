/*
 *  ExponentialFitter.h
 *  ScheduleModelBuilder
 *
 *  Created by Peter Sels on 13/07/09.
 *
 */

#include <vector>
#include <map>

#include "Exponential.h"

class Histogram;

// two data series are given: 
// xi for i:0..N-1
// yi for i:0..N-1
// result is the function y=f(x)=a*e^(bx) that fits best
// using the method of the leas square sum of the logaritmic residuals.
class ExponentialFitter {
private:
	int N_;
	double a_;
	double b_;
public:
	ExponentialFitter();
  Exponential match(
    const Histogram & histogram) const;
	Exponential fit(
    const std::vector<double> & x, 
    const std::vector<double> & y);
  Exponential fit(
    const std::map<double, double> & distribution);
  Exponential fit( // internally converting to doubles
    const std::map<unsigned int, unsigned int> & distribution);
	~ExponentialFitter();
};
