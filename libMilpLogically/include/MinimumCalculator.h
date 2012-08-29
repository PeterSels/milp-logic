#ifndef MINIMUM_CALCULATOR_H
#define MINIMUM_CALCULATOR_H

#include <vector>

class MinimumCalculator {
public:
  MinimumCalculator(double (*fPtr)(const std::vector<double>
                                    & parameters, 
                                    double d),
                    const std::vector<double> & parameters,
                    unsigned int D1,
                    double * curve,
                    unsigned int size);
  double getMinimumAbsis() const;
  double getMinimumValue() const;
  ~MinimumCalculator();

private:
  const double D1_;
        double dMin_;
        double zMin_;  
};

#endif // MINIMUM_CALCULATOR_H

