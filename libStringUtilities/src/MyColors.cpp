#include <assert.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

#include "MyColors.h"

using namespace std;

Rgb::Rgb()
: r_(0)
, g_(0)
, b_(0)
{
}

Rgb::Rgb(unsigned int r, unsigned int g, unsigned int b) 
: r_(r)
, g_(g)
, b_(b)
{
}

string Rgb::toString(const std::string format) const {
  stringstream strStr;
  if (format=="svg") {
    strStr << "rgb("
    << r_ << "," 
    << g_ << "," 
    << b_ << ")";     
  } else if (format=="R") {
    strStr << "\"#"
    << hex << setw(2) << setfill('0') << noshowbase << r_ 
    << hex << setw(2) << setfill('0') << noshowbase << g_ 
    << hex << setw(2) << setfill('0') << noshowbase << b_ 
    << "\"" << dec;
  } else {
    cerr << "ERROR: Illegal rgb output format:" << format << endl;
    assert(false);
    exit(0);
  }
  
  string str = strStr.str();
  return str;
}

Rgb::~Rgb() {
}

////////////////////

/*
 #define MY_RED          "rgb(255,0,0)"
 #define MY_LIGHT_RED    "rgb(255,95,95)"
 
 #define MY_BLUE         "rgb(0,127,255)"
 #define MY_LIGHT_BLUE   "rgb(63,191,255)"
 
 #define MY_GREEN        "rgb(0,127,0)"
 #define MY_LIGHT_GREEN  "rgb(63,255,63)"
 
 #define MY_ORANGE       "rgb(255,127,0)"
 #define MY_LIGHT_ORANGE "rgb(255,191,63)"
 
 #define MY_YELLOW       "rgb(255,255,0)"
 #define MY_LIGHT_YELLOW "rgb(255,255,191)"
 
 #define MY_RED_R "#\"ff0000\""
 */

MyColors::MyColors() {
  addColor("RED",          Rgb(255,   0,   0));
  addColor("LIGHT_RED",    Rgb(255,  95,  95));
  
  addColor("BLUE",         Rgb(  0, 127, 255));
  addColor("LIGHT_BLUE",   Rgb( 63, 191, 255));
  
  addColor("GREEN",        Rgb(  0, 127,   0));
  addColor("LIGHT_GREEN",  Rgb( 63, 255,   63));
  
  addColor("ORANGE",       Rgb(255, 127,    0));
  addColor("LIGHT_ORANGE", Rgb(255, 191,   63));
  
  addColor("YELLOW",       Rgb(255, 255,    0));
  addColor("LIGHT_YELLOW", Rgb(255, 255,   191));
  
  addColor("PURPLE",       Rgb(200,  63,   200));
  addColor("LIGHT_PURPLE", Rgb(230, 191,   230));
  
  addColor("GREY",         Rgb(127, 127,   127));
  addColor("LIGHT_GREY",   Rgb(200, 200,   200));


}

void MyColors::addColor(const std::string colorName, const Rgb rgb) {
  assert(colors_.count(colorName)==0);
  colors_[colorName] = rgb;
}

const Rgb & MyColors::getColor(const std::string colorName) const {
  assert(colors_.count(colorName)==1);
  const Rgb & rgb = colors_.find(colorName)->second;
  return rgb;
}

string MyColors::getColor(const std::string colorName, 
                          const std::string format) const {
  if (colors_.count(colorName)!=1) {
    assert(colors_.count(colorName)==1);
  }
  const Rgb & rgb = colors_.find(colorName)->second;
  string s = rgb.toString(format);
  return s;
}

MyColors::~MyColors() {
}


