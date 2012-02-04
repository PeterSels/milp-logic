#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <map>
#include <string>
#include <vector>

class Histogram : public std::map<int, double> {
public:
  Histogram(unsigned int storeBucketSize=1);
  Histogram(const std::vector<int> & values, unsigned int storeBucketSize=1);
  //Histogram(const DataMap & dataMap);
  void setStoreBucketSize(unsigned int storeBucketSize);
  void incrementOccurrenceOfX(int x);
  void incrementOccurrenceOfXByY(int x, double y);
  void addPointXY(int x, double y);
  
  int calcClosestBucketMiddleToX(int x) const;
  
  void inventDistributionWhenAbsent();
  
  int calcXMin() const;
  int calcXMax() const;
  // calcYMin would always give 0 in a general histogram
  double calcYMax() const; // depends on unsigned int xResolution
  double getYForX(int x) const;
  
  double calcTotal() const;
  double calcAverage() const;
  int calcMedian() const;
  std::string toString() const;
  std::string toRStringHeader(std::string fileName,
                              bool isTopLevel) const;
  std::string toRStringRest(std::string fileName, 
                            std::string xLabel, 
                            std::string yLabel,
                            std::string title,
                            int xMin,
                            int xMax,
                            int xStep,
                            double yMax,
                            std::string language) const;
  ~Histogram();

private:
  unsigned int storeBucketSize_;
};

#endif // HISTOGRAM_H
