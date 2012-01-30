#ifndef DATA_VECTOR_CORRELATOR_H
#define DATA_VECTOR_CORRELATOR_H

#include <vector>
#include <iostream>

class DataVectorCorrelator {
 public:
  DataVectorCorrelator(const std::vector<double> & v0,
                       const std::vector<double> & v1);
  double getMean(unsigned int i) const;
  double getVariance(unsigned int i) const;
  double getStandardDeviation(unsigned int i) const; // = sigma = sqrt(variance)
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
  double standardDeviation_[2];
  double covariance_;
  double correlation_;
  double a_;
  double b_;
};

#endif // DATA_VECTOR_CORRELATOR_H
