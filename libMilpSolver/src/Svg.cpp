#include <fstream>

#include "Svg.h"

using namespace std;

Svg::Svg(const string & fileName,
		 unsigned int width, unsigned int height)
: fileName_(fileName)
, width_(width)
, height_(height)
{
	addHeader();
}

void Svg::addHeader() {
  strstr_ << "<?xml version='1.0' standalone='no'?>\n";
  strstr_ << "<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN'\n";
  strstr_ << "'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n";
	
  strstr_ << "<svg " << "version='1.1' baseProfile=\"full\"" << endl;
	strstr_ << "  xmlns='http://www.w3.org/2000/svg'" << endl;
	strstr_ << "  xmlns:xlink='http://www.w3.org/1999/xlink'" << endl;
	strstr_ << "  xmlns:ev='http://www.w3.org/2001/xml-events'" << endl;
	strstr_ << "  width='" << width_ << "px'" << endl; 
	strstr_ << "  height='" << height_ << "px'" << endl;
	strstr_ << ">" << endl;	
  strstr_ << "<desc>" << fileName_ << "</desc>\n";
}

void Svg::addLine(
  unsigned int x1, unsigned int y1,
  unsigned int x2, unsigned int y2,
  unsigned int thickness,
  const string color,
	const string dashArray,
  const string title) {

  strstr_ << "<g fill='none' stroke='" << color
         <<"' stroke-width='" << thickness << "' ";
	if (dashArray != SOLID_DASH_ARRAY) {
	  strstr_ << "stroke-dasharray='" << dashArray << "'"; 
	}
	strstr_ << ">" << endl;
  strstr_ << "<line x1='" << x1 << "' y1='" << y1
    << "' x2='" << x2 << "' y2='" << y2 << "'>" << endl;	
	if (title!="") {
		strstr_ << "    <title>" << title << "</title>" << endl;
	}
	strstr_ << "</line>\n";
	
  strstr_ << "</g>\n";
}

void Svg::addCircle(
  unsigned int cx, unsigned int cy,
  unsigned int radius,
  unsigned int strokeWidth,
  const string strokeColor,
  const string fillColor) {

  strstr_ << "<circle cx='" << cx << "' cy='" << cy << "' r='" 
	<< radius << "'" << endl;
  strstr_ << "stroke='" << strokeColor
    << "' stroke-width='" << strokeWidth << "' fill='" << fillColor 
	<< "'/>" << endl;
}

void Svg::addRectangle(unsigned int x,
											 unsigned int y,
											 unsigned int width, 
											 unsigned int height,
											 unsigned int strokeWidth,
											 const string strokeColor,
											 const string fillColor,
											 float strokeOpacity,
											 float fillOpacity,
											 string title,
											 string id,
											 string xlinkTo) {
   
  if (xlinkTo != "") {
		strstr_ << "<a xlink:href=\"" 
		<< fileName_ 
		<< "#" << xlinkTo 
		<< "\"" << ">" 
		<< endl;
	}
	strstr_
	<< "  <rect ";
	
	if (id!="") {
    strstr_ << "id=" << "\"" << id << "\"" << endl;
	}
	
	strstr_
	<< "x='" << x << "' y='" << y 
	<< "' width='" << width
	<< "' height='" << height
	<< "' style='fill:" << fillColor 
	<< ";stroke:" << strokeColor 
	<< ";stroke-width:" << strokeWidth << ";"
	<< "fill-opacity:" << fillOpacity 
	<< ";stroke-opacity:" << strokeOpacity << "'" 
	<< "> " << endl;
	
	if (title!="") {
		strstr_ << "    <title>" << title << "</title>" << endl;
	}
		
	strstr_	
	<< "  </rect>" << endl;	 

  if (xlinkTo != "") {
		strstr_ << "</a>" << endl;
	}
	strstr_ << endl;
}

void Svg::addText(
  unsigned int x, unsigned int y,
  const string & text,
  const string color,
  unsigned int fontSize,
  const string fontName,
  unsigned int angle) {

  strstr_ << "<g font-size='" << fontSize << "' font-family='" 
	<< fontName << "' >\n";
  strstr_ << "<text ";
  strstr_ << " style='stroke:none; fill:" << color << ";'";
  strstr_ << " x='" << x << "' y='" << y << "'";
  strstr_ << " transform='rotate(" << angle << ", " << x << ", " << y << ")'";
  strstr_ << ">";
  strstr_ << text << "</text></g>" << endl;
}

void Svg::addSvgString(const string & svgString) {
  strstr_ << svgString;
}

std::ostringstream & Svg::getStream() {
  return strstr_;
}

void Svg::addFooter() {
  strstr_ << "</svg>" << endl;
}

void Svg::close() {
  addFooter();

  ofstream ofStr(fileName_.c_str());
  ofStr << strstr_.str();
  ofStr.close();
}

Svg::~Svg() {
}
