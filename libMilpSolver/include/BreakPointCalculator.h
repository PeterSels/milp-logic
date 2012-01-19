#ifndef BREAK_POINT_CALCULATOR_H
#define BREAK_POINT_CALCULATOR_H

#include <vector>

class BreakPointCalculator {
public:
  BreakPointCalculator(double (*fPtr)(const std::vector<double>
                                      & parameters, 
                                      double d),
                       const std::vector<double> & parameters,
                       unsigned int D1,
                       bool fDecreasing,
                       unsigned int dBrk);
  double getBreakPointAbsis() const;
  double getBreakPointValue() const;
  ~BreakPointCalculator();

private:
  const double D1_;
        double dBrk_;
        double zBrk_;  
};

#endif // BREAK_POINT_CALCULATOR_H

