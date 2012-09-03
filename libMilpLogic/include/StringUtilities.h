#ifndef STRING_UTILITIES_H
#define STRING_UTILITIES_H

#include <string>
#include <vector>
#include <iostream>

std::string characterToString(char ch);
int toInt(const std::string & str);
double toDouble(const std::string & str);

std::string escapeUnderscore(const std::string & str);

std::string toString(double f,
                     unsigned int fracDigits, 
                     unsigned int width);
std::string stringToUpper(std::string & str);
std::string stringToLower(std::string & str);
void removeDotsFromString(std::string & str);
bool removeFirstDotFromString(std::string & str);

std::vector<std::string> splitStringOnFirstChar(const std::string & str, 
																								char ch);
std::vector<std::string> splitStringOnFirstChar(const std::string & str, 
																								const std::string & substr);

std::vector<std::string> splitStringOnChar(std::vector<std::string> & v, 
																					 char ch);
std::vector<std::string> splitStringOnChar(const std::string & str, char ch);
std::vector<std::string> splitStringOnString(const std::string & str,
                                             const std::string & substr);

std::string readStringUpTo(std::istream & istr, char ch);
int readIntUpTo(std::istream & istr, char ch);
double readDoubleUpTo(std::istream & istr, char ch);

void replaceAllOddByEven(std::string & str, const std::string & oddEven);
void replaceAll(std::string & str, char oldCh, char newCh);
bool replace(std::string & str, char oldCh, char newCh);

void parseWhiteSpace(std::istream & istr);
void parseWhiteSpaceAndOneLineCComment(std::istream & istr);

bool ifCanReadFromWriteTo(const std::string & fileName, std::ostream & ostr);

#endif // STRING_UTILITIES_H
