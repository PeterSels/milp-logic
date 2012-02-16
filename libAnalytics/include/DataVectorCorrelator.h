#ifndef DATA_VECTOR_CORRELATOR_H
#define DATA_VECTOR_CORRELATOR_H

#include <vector>
#include <iostream>

#undef DATA_CORRELATOR_DO_STD_DEV

bool isEqual(double a, double b, double tolerance);

class DataVectorCorrelator {
 public:
  DataVectorCorrelator(const std::vector<double> & v0,
                       const std::vector<double> & v1);
  
  void init();
  DataVectorCorrelator(unsigned int firstPointIndex,
                       unsigned int nPoints, 
                       double xStep, 
                       double * yValues);
  
  double getMean(unsigned int i) const;
  double getVariance(unsigned int i) const;
#ifdef DATA_CORRELATOR_DO_STD_DEV    
  double getStandardDeviation(unsigned int i) const; // = sigma = sqrt(variance)
#endif
  double getCovariance() const;
  double getCorrelation() const;
  double getSlope() const; // b in y = b*x + a
  double getAbsis() const; // a in y = b*x + a
  
  const std::string getName() const;
  bool txtWriteMembers(std::ostream & txtOStream) const;

  ~DataVectorCorrelator();
 private:
  double mean_[2];
  double variance_[2];
#ifdef DATA_CORRELATOR_DO_STD_DEV  
  double standardDeviation_[2];
#endif  
  double covariance_;
#ifdef DATA_CORRELATOR_DO_STD_DEV  
  double correlation_;
#endif  
  double a_;
  double b_;
};

#endif // DATA_VECTOR_CORRELATOR_H
