#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <fstream>
#include <assert.h>

#include "StringUtilities.h"
#include "NUnits.h"

using namespace std;


std::string characterToString(char ch) {
  string str;
  str.push_back(ch);
  return str;
}

// for Latex
// FIXME: For now, supposing there is only one underscore 
// per string argument str
string escapeUnderscore(const string & str) {
  string news = str;
  unsigned int npos = (unsigned int)str.length();
  unsigned int pos = (unsigned int)str.find('_');
  if (pos<npos) {
    news.insert(pos, "\\");
  }
  return news;
}

string toTime_HH_MM(unsigned int units) {
  string str = toTime_HH_MM_SS(units);
  str = str.substr(0, 5);
  return str;
}

string toTime_HH_MM_SS(unsigned int units) {
  ostringstream oStrStr;
  unsigned int rest = units;
  
  unsigned int hh = rest / UU_PER_HR;
  rest -= hh * UU_PER_HR;
  
  unsigned int mm = rest / UU;
  rest -= mm * UU;
  
  unsigned int ss = rest / 1;
  rest -= ss * 1;
  
  assert(rest == 0);
  
  oStrStr << setw(2) << setfill('0') << hh << ":" 
          << setw(2) << setfill('0') << mm << ":" 
          << setw(2) << setfill('0') << (ss * (60 / UU));
  string str = oStrStr.str();
  unsigned int len = (unsigned int)str.length();
  assert(len==8);
  return str;
}

string toString(double f,
                unsigned int fracDigits, 
                unsigned int width) {
  ostringstream oStrStr;
  oStrStr << setw(width) << setfill('0') 
    << fixed << setprecision(fracDigits) << f;
  string str = oStrStr.str();
  return str;
}


string stringToUpper(string & str) {
  for(unsigned int i=0; i<str.length(); i++) {
    str[i] = toupper(str[i]);
  }
  return str;
}

string stringToLower(string & str) {
  for(unsigned int i=0; i<str.length(); i++) {
    str[i] = tolower(str[i]);
  }
  return str;
}


void removeDotsFromString(string & str) {  
  while (removeFirstDotFromString(str)) {};  
}

bool removeFirstDotFromString(string & str) {  
  unsigned int beyond = (unsigned int)str.length();
  unsigned int pos = (unsigned int)str.find('.');
  if (pos >= beyond) {
    return false;
  } else {
    string pre = str.substr(0, pos);
    string post = str.substr(pos+1); 
    str = pre + post;
    return true;
  }
}

vector<string> splitStringOnFirstChar(const string & str, char ch) {
  vector<string> v;
  unsigned int beyond = (unsigned int)str.length();
  unsigned int pos = (unsigned int)str.find(ch);
  if (pos >= beyond) {
    v.push_back(str);
  } else {
    string pre = str.substr(0, pos);
    string post = str.substr(pos+1); 
    v.push_back(pre);
    v.push_back(post);
  }
  return v;
}

bool splitStringVectorBackOnChar(vector<string> & v, char ch) {
  string str = v.back();
  
  vector<string> v1 = splitStringOnFirstChar(str, ch);
  if (v1.size()==1) {
    // leave vector v unchanged
    return false;
  } else {
    // replace back string by 2 strings
    v.pop_back();
    v.push_back(v1[0]);
    v.push_back(v1[1]);
    return true;
  }
}

vector<string> splitStringOnChar(const string & str, char ch) {
  vector<string> v;
  v.push_back(str);
  bool replaced = false;
  do {
    replaced = splitStringVectorBackOnChar(v, ch);
  } while (replaced);
  return v;
}

string readStringUpTo(istream & istr, char ch) {
  string token;
  if (getline(istr, token, ch)) {
    return token;
  } else {
    assert(false);
    exit(0);
  }
}

int readIntUpTo(istream & istr, char ch) {
  string token = readStringUpTo(istr, ch);
  stringstream ss;
  ss << token;
  int i;
  ss >> i;
  return i;
}

double readDoubleUpTo(istream & istr, char ch) {
  string token = readStringUpTo(istr, ch);
  stringstream ss;
  ss << token;
  double d;
  ss >> d;
  return d;
}

void replaceAllOddByEven(string & str, const string & oddEven) {
  unsigned int oeLen = (unsigned int)oddEven.length();
  assert(oeLen % 2 == 0);
  for (unsigned int i=0; i<oeLen; i+=2) {
    char oldCh = oddEven[i];
    char newCh = oddEven[i+1];
    replaceAll(str, oldCh, newCh);
  }
}

void replaceAll(string & str, char oldCh, char newCh) {
  bool replaced = false;
  do {
    replaced = replace(str, oldCh, newCh);
  } while (replaced);
}

bool replace(string & str, char oldCh, char newCh) {
  unsigned int beyond = (unsigned int)str.length();
  unsigned int pos = (unsigned int)str.find(oldCh);
  if (pos >= beyond) {
   return false;
  } else {
    str[pos] = newCh;
    return true;
  }
}

int toInt(const string & str) {
  stringstream strstr;
  strstr << str;
  int i;
  strstr >> i;
  return i;
}

double toDouble(const string & str) {
  stringstream strstr;
  strstr << str;
  double d;
  strstr >> d;
  return d;
}

void parseWhiteSpace(istream & istr) {
	char ch;
  do {
		istr >> ch;
	} while (isspace(ch));
	istr.unget();
}

void parseWhiteSpaceAndOneLineCComment(istream & istr) {
	char ch;
	bool stop = false; // corersponds to readTooFar
	do {
		// read until non white space
		do {
			istr >> ch;
		} while (isspace(ch));

		if (ch=='/') {
			// read whole one line C comment
			istr >> ch;
			if (ch=='/') {
				do {
					//istr >> ch; // skips isspace internally, while we need to catch '\n'
					istr.get(ch); // does not skip anything and will show up with ''n'
				} while (ch!='\n'); // supposing transparently translated to correct 
				// CR/CR+LF/LF code(s) in Win/*nix systems
			} else {
				istr.unget(); // puts back non '/'
				istr.unget(); // puts back first '/'
				stop = true;
			}
		} else {
			istr.unget();
			stop = true;
		}		
	} while (!stop);
	// POST: can continue to read with other parsing functions, 
	// trying to recognize unknown continuation at cursor
}

bool ifCanReadFromWriteTo(const string & fileName, ostream & ostr) {
	ifstream ifstr(fileName.c_str());
	if (!ifstr) {
		cerr << "WARNING: Cannot open file '" << fileName << "'." << endl;
		cerr << "Skipping. OK." << endl;
		return false;
	} else {
		string line;
		while (getline(ifstr, line)) {
			ostr << line << endl;
		}		
		ifstr.close();
		return true;
	}
} 







