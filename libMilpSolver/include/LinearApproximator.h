#ifndef LINEAR_APPROXIMATOR_H
#define LINEAR_APPROXIMATOR_H

#include <vector>

class LinearApproximator {
public:
  LinearApproximator();
  LinearApproximator(double (*fPtr)(const std::vector<double>
                                    & parameters, 
                                    int ii),
                     const std::vector<double> & parameters,
                     unsigned int D1);
  double eval(double d) const;
  double getD(unsigned int i) const;
  double getZ(unsigned int i) const;
  ~LinearApproximator();

private:
  // 0
  double z0_;
    
  double D1_;
  double zD1_;
  
  // non const
  double dMin_;
  double zMin_;
};

#endif // LINEAR_APPROXIMATOR_H

