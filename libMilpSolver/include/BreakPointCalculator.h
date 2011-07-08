#ifndef BREAK_POINT_CALCULATOR_H
#define BREAK_POINT_CALCULATOR_H

#include <vector>

class BreakPointCalculator {
public:
  BreakPointCalculator(double (*fPtr)(const std::vector<double>
                                    & parameters, 
                                    int ii),
                    const std::vector<double> & parameters,
                    unsigned int D1);
  double getBreakPointAbsis() const;
  double getBreakPointValue() const;
  ~BreakPointCalculator();

private:
  const double D1_;
        double dBrk_;
        double zBrk_;  
};

#endif // BREAK_POINT_CALCULATOR_H

