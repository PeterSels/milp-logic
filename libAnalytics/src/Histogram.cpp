#include <assert.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <stdlib.h>

#include "Histogram.h"


using namespace std;

Histogram::Histogram(unsigned int storeBucketSize)
: storeBucketSize_(storeBucketSize)
{
}

void Histogram::setStoreBucketSize(unsigned int storeBucketSize) {
  storeBucketSize_ = storeBucketSize;
}

/*
Histogram::Histogram(const DataMap & dataMap) 
  //: DataMap(dataMap.getResolution())
{
  
  cerr << "ERROR: still to convert datamap to Histogram or to discard Histogram" << endl;
  assert(false);
  exit(0);

  
	unsigned int dataYMin = 0; // dataMap.calcYMin() = 1 'coz 0s not stored
	unsigned int dataYMax = dataMap.calcYMax();
	cout << "[dataYMin, dataYMax] = [" << dataYMin << ", " << dataYMax << "]" << endl;
	unsigned int dataXMin = dataMap.getXMin();
	unsigned int dataXMax = dataMap.getXMax();
	//unsigned int resolution = dataMap.getResolution();
  for (unsigned int dataX=dataXMin; dataX<=dataXMax; dataX+=resolution_) {
		unsigned int dataY = dataMap.getYForX(dataX);
		unsigned int x = dataY;
		add(x, 1);
	}
}
*/

Histogram::Histogram(const std::vector<int> & values, 
                     unsigned int storeBucketSize) 
: storeBucketSize_(storeBucketSize)
{
  unsigned int n = (unsigned int)values.size();
  for (unsigned int i=0; i<n; i++) {
    incrementOccurrenceOfX(values[i]);
  }
}

void Histogram::incrementOccurrenceOfX(int x) {
  incrementOccurrenceOfXByY(x, 1.0);
}

void Histogram::incrementOccurrenceOfXByY(int x, double y) {  
  int closeToX = calcClosestBucketMiddleToX(x);
  ((*this)[closeToX])+=y;
}

void Histogram::addPointXY(int x, double y) { 
  int closeToX = calcClosestBucketMiddleToX(x);
  unsigned int cnt = (unsigned int)count(closeToX);
  assert(cnt == 0); // only if not present yet
  (*this)[closeToX] = y;
}

int Histogram::calcClosestBucketMiddleToX(int x) const {
  if (x < 0) {
    cerr << "ERROR: x=" << x << " cannot be negative" << endl;
  }
  assert(x >= 0); // for our current application... // FIXME  
  int bucketNr = (int)( (double)x / (double)storeBucketSize_ ); 
  // correctly treats fractional part
  int closeToX = (int)((bucketNr+0.5) * storeBucketSize_); // 0.5 for bucket middle
  return closeToX;
}


// hmm:... FIXME
void Histogram::inventDistributionWhenAbsent() {
  bool verbose = false;
  if (verbose) {
    cerr << "ERROR: Histogram::inventDistributionWhenAbsent "
    << "shouldn't be called at all" << endl;
  }
  //assert(false);
  
  //cout << "histogram:A = " << endl << toString() << endl;
  
  unsigned int nx = (unsigned int)size();
  if (nx==0) {
    if (verbose) {
      cout << "  Assuming reasonable delays of "
           << "twice zero delay and once unit delay." << endl;
    }
    cout << "WARNING: Data absent. Creating 2*0 + 1*1 distribution." << endl;
    (*this)[0] = 2;
    (*this)[1] = 1;
  } else if (nx==1) { // usually all zeroes, 
    int x = begin()->first;
    unsigned int y = begin()->second; // FIXME: y should de double now...
    // single sample is not much, but theoretically enough to fit exponential
    if (verbose) {
      cout << "WARNING: " << y << " times same sample = " << x << endl;
    }
    
    if (count(0)>0) { // if only (x,y) present = (0, *), cannot fit exponential
      if (verbose) {
        cout << "  All delay samples coincide into one x value." << endl;
        cout << "  Splittig equally into two x and x+1 delay to allow fitting." 
        << endl;
      }
      unsigned int extra = (y/2==0) ? 1 : 0;
      (*this)[x]   = y/2 + (y%2) + extra;
      (*this)[x+1] = y/2 + extra;
      assert((*this)[x] + (*this)[x+1] == y+(2*extra));
    }
  } else {
    cout << "NOTE: " << nx << " different x values present. Ok." << endl;
  }
  //cout << "histogram:B = " << endl << toString() << endl;

  nx = size();
  //assert(nx==2);
  assert(nx>=1);
}

double Histogram::calcTotal() const {
  double total = 0.0;
  if (size()>0) {
    for (Histogram::const_iterator it =begin(); it!=end(); it++) {
      int x = it->first;
      double y = it->second;
      total += x * y; // all x are stored as middle of buckets already
    }
  }
  return total;
}

double Histogram::calcAverage() const {
  double sum = 0;
  double n   = 0;
  double avg = 0.0;
  if (size()>0) {
    for (Histogram::const_iterator it =begin(); it!=end(); it++) {
      int x = it->first;
      double y = it->second;
      sum += x * y; // all x are stored as middle of buckets already
      n   +=     y;
    }
    avg = sum/n;
  }
  return avg;
}

// only works if y values are ints (io doubles)
int Histogram::calcMedian() const {
  double nSamples = 0;
  for (Histogram::const_iterator it =begin(); it!=end(); it++) {
    double y = it->second;
    nSamples += y;
  }
  double midNSamples = nSamples / 2.0;
  assert(midNSamples <= nSamples);
  nSamples = 0; // again
  for (Histogram::const_iterator it =begin(); it!=end(); it++) {
    double y = it->second;
    nSamples += y;
    if (nSamples >= midNSamples) {
      int x = it->first;
      return x;
    }
  }
  assert(false);
  cerr << "ERROR: In Histogram::calcMedian()" << endl;
  return 0;
}

int Histogram::calcXMin() const {
  int xMin = numeric_limits<int>::max();
  
  for (map<int, double>::const_iterator
       it =begin(); it!=end(); it++) {
    int x = it->first; // all x are stored as middle of buckets already
    xMin = min<int>(xMin, x);
  }
  
  return xMin;
}

int Histogram::calcXMax() const {
  int xMax = numeric_limits<int>::min();
  
  for (Histogram::const_iterator it =begin(); it!=end(); it++) {
    int x = it->first; // all x are stored as middle of buckets already
    xMax = max<int>(xMax, x);
  }

  return xMax;
}

// calcYMin would always give 0 in a general histogram
// so absent 

double Histogram::calcYMax() const {

  unsigned int xResolution = storeBucketSize_;
  
  assert(xResolution >= storeBucketSize_);
  assert(xResolution % storeBucketSize_ == 0); // I think .. to avoid lossiness? CHECKME
  
  Histogram printHistogram(xResolution);
  
  // redistribute this histogram in buckets of xResolution wide
  for (Histogram::const_iterator it =begin(); it!=end(); it++) {
    int x = it->first;
    double y = it->second;
    // since multiple x are mapped into on x' bucket
    // we cannot use addPointXY here
    printHistogram.incrementOccurrenceOfXByY(x, y);
  }
  
  // calc maximum of (heigher) heights (y) for these larger buckets
  double yMax = 0;
  for (Histogram::const_iterator it =printHistogram.begin();
       it!=printHistogram.end(); it++) {
    double y = it->second;
    yMax = max<double>(yMax, y);
  }

  return yMax;
}

double Histogram::getYForX(int x) const {
  if (count(x)==0) {
    return 0.0;
  } else {
    return this->find(x)->second;
  }
}


string Histogram::toString() const {
  ostringstream strstr;
  for (Histogram::const_iterator it=begin(); it!=end(); it++) {
    int x = it->first;
    double y = it->second;
    strstr
      << setw(10) << fixed << x 
      << " -> " 
      << setw(10) << fixed << y << endl;
  }
  return strstr.str();
}


string Histogram::toRStringHeader(string fileName,
                                  bool isTopLevel) const {
  ostringstream strstr;
  
  if (isTopLevel) {
    strstr << "require(graphics)" << endl << endl;
  }
  
  unsigned int nRows = 1;
  unsigned int nCols = 1;
  
  if (isTopLevel) {
    string pdfFileName = fileName + ".pdf";
    strstr
    << "pdf(file='" << pdfFileName 
    << "', height=" << nRows << "*8, width=" << nCols << "*8, onefile=TRUE)" 
    << endl << endl;  
  }  
  return strstr.str();
}

string Histogram::toRStringRest(string fileName, 
                                string xLabel, 
                                string yLabel,
                                string title,
                                int xMin,
                                int xMax,
                                int xStep,
                                double yMax,
                                string language) const {
  bool first = true;
  ostringstream strstr;
  
  strstr << xLabel << " <- c(";
  for (Histogram::const_iterator it=begin(); it!=end(); it++) {
    int x = it->first;
    double y = it->second;
    
    bool printx = (y!=0.0); // double == is dangerous I know... 
    //but somehow useless zeroes crept in
    
    if (printx) {
      if (first) {
        first = false;
      } else {
        strstr << "," << endl;
      }
      strstr << "  rep(" << x << ", " 
      << setprecision(0)  // should be param instead : FIXME
      << fixed 
      << y << ")";
    }
  }
  strstr << ")" << endl;
  
  int newXMin = xMin;
  double newXStep = xStep;
  int newXMax = xMax; 
  
  double newYMax = yMax;
  
  /*
  const bool newLimits = false;
  if (newLimits) {
    newXMin  = 0;
    newXMax  = (int)((double)xMax * 1.2);
    newXStep = //(int)
  ((newXMax - xMin)/60.0); // 60 steps
    
    newYMax  = yMax * 1.2;
  }
  */

  if (newXStep == 0) {
    cerr << "ERROR: newXStep cannot be 0" << endl;
    assert(false);
    exit(0);
  }
  
  // guarantee an integer nr of steps is between
  // newXMin and newXMax
  newXMax = (int)(newXMin +
  ((int)(newXMax - newXMin) / (int)newXStep + 1) * newXStep);
  
  strstr << "hist(" << xLabel
     << ", breaks=seq(" << newXMin << ", " << newXMax 
  
  << ", by=" 
  //<< xStep 
    << newXStep 
 
    << ")"
    << ", main='" << title 
    << "', col='grey'" 
    << ", ylim=c(0, " << newYMax << ")" 
    << ", ylab='" << yLabel << "'" 
    << ")" << endl;
  
  double tot = calcTotal(); // x
  
  double avg = calcAverage(); // x
  strstr << "abline(v=" << avg << ", col='blue', lty='4222', lwd=1)" 
  << endl << endl;

  double xText = newXMax/2;

  double yTotText = (double)yMax * 0.9;
  strstr << "text(" << xText << ", " << yTotText << ", 'tot=" 
  << setprecision(2) << fixed << tot << "', cex=1.4, col='blue')" 
  << endl << endl;

  double yAvgText = (double)yMax * 0.6;
  strstr << "text(" << xText << ", " << yAvgText << ", '" 
  << ((language=="NL") ? "gem" : "avg") << "=" 
  << setprecision(2) << fixed << avg << "', cex=1.4, col='blue')" 
  << endl << endl;
  
  
  strstr << "box()" << endl << endl;
  
  return strstr.str();
}

Histogram::~Histogram() {
}
