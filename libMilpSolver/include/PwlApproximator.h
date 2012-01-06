#ifndef PWL_APPROXIMATOR_H
#define PWL_APPROXIMATOR_H

#include <vector>

class PwlApproximator {
public:
  PwlApproximator(bool brkPointNotMinimum);
  PwlApproximator(bool brkPointNotMinimum,
                  double (*fPtr)(const std::vector<double>
                                 & parameters, 
                                 double d),
                  const std::vector<double> & parameters,
                  unsigned int D1);
  
  void crossingLinesIntersect(double & x, double & y,
                              double absis0, double slope0,
                              double absis1, double slope1);
  bool getBrkPointNotMinimum() const;
  void setBrkPointNotMinimum(bool brkPointNotMinimum);
  double eval(double d) const;
  double getD(unsigned int i) const;
  double getZ(unsigned int i) const;
  ~PwlApproximator();

private:
  // 0
  double z0_;
    
  double D1_;
  double zD1_;
  
  // non const
  double dMin_;
  double zMin_;
  
  bool brkPointNotMinimum_;
};

#endif // PWL_APPROXIMATOR_H

