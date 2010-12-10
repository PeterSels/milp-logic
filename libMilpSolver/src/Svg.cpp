#include <fstream>

#include "Svg.h"
#include "StringUtilities.h"

#define CROP_BORDER (1000)

using namespace std;

Svg::Svg(const string & fileName,
		 unsigned int width, unsigned int height)
: fileName_(fileName)
, width_(width)
, height_(height)
, minX_(width-1)
, maxX_(0)
, minY_(height-1)
, maxY_(0)
{
}


void Svg::setWidth(unsigned int width) {
	width_ = width;
}

void Svg::setHeight(unsigned int height) {
  height_ = height;
}

string Svg::getHeader() const {
	stringstream headerStr;
  headerStr << "<?xml version='1.0' standalone='no'?>\n";
  headerStr << "<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN'\n";
  headerStr << "'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n";
	
  //headerStr << "<svg "
  headerStr << "<svg onload='centerWindow()' "
	  << "version='1.1' baseProfile=\"full\"" << endl;
	headerStr << "  xmlns='http://www.w3.org/2000/svg'" << endl;
	headerStr << "  xmlns:xlink='http://www.w3.org/1999/xlink'" << endl;
	headerStr << "  xmlns:ev='http://www.w3.org/2001/xml-events'" << endl;
	headerStr << "  width='" << width_ << "px'" << endl; 
	headerStr << "  height='" << height_ << "px'" << endl;
	//headerStr << "  title='" << fileName_ << "'" << endl;
	headerStr << ">" << endl;	
  headerStr << "<g><title>" << fileName_ << "</title></g>" << endl;
	
	
	headerStr << "<script type=\"text/ecmascript\"><![CDATA[" << endl;
	readFromWriteTo("../Scroll.js", headerStr);
	/*
	headerStr << "  function centerWindow() {" << endl;
	headerStr << "    var mx = window.screen.width / 2;" << endl;
	headerStr << "    var my = window.screen.height / 2;" << endl;
	headerStr << "    window.scroll(mx, my)" << endl;
	headerStr << "  }" << endl;
	*/
	headerStr << "]]></script>" << endl;

	
	/*
	 headerStr <<
	 "function include(file) {" << endl <<
	 "  if (document.createElement && document.getElementsByTagName) {" << endl <<
	 "    var head = document.getElementsByTagName('defs')[0];" << endl <<
	 "    var script = document.createElement('script');" << endl <<
	 "    script.setAttribute('type', 'text/javascript');" << endl <<
	 "    script.setAttribute('src', file);" << endl <<	
	 "    head.appendChild(script);" << endl <<
	 "  } else {" << endl <<
	 "    alert('Your old browser cannot deal with the DOM standard. Update it!');" 
	 << endl <<
	 "  }" << endl <<
	 "}" << endl;
	 
	 headerStr << "include('../Scroll.js');" << endl;
	 
	 */  
	
	/*
	headerStr << "  function jumpFromSrcIdToDstId(srcId, dstId) {" << endl;
	//headerStr << "    var svgDoc = evt.target.ownerDocument;" << endl;
	//headerStr << "    var x = svgDoc.getElementById(thatId).x;" << endl;
	headerStr << "    var srcX = document.getElementById(srcId).x.baseVal.value;" 
	<< endl;
	headerStr << "    var srcY = document.getElementById(srcId).y.baseVal.value;" 
	<< endl;
	headerStr << "    var dstX = document.getElementById(dstId).x.baseVal.value;" 
	<< endl;
	headerStr << "    var dstY = document.getElementById(dstId).y.baseVal.value;" 
	<< endl;
	headerStr << "    var dx = dstX - srcX;" << endl;
	headerStr << "    var dy = dstY - srcY;" << endl;
	headerStr << "    window.zoomIndependentScrollByLogicalPixels(dx,dy);" 
	<< endl;
	headerStr << "  }" << endl;
	
	headerStr << 
	"function getZoomFactor() {" << endl <<
  "	var factor = 1;" << endl <<
	"	if (document.getBoundingClientRect) {" << endl <<
	"		// rect is only in physical pixel size in IE before version 8 " << endl <<
	"		var rect = document.getBoundingClientRect();" << endl <<
	"		var physicalW = rect.right - rect.left;" << endl <<
	"		var logicalW = document.offsetWidth;" << endl <<
	"		// the zoom level is always an integer percent value" << endl <<
	"		factor = Math.round ((physicalW / logicalW) * 100) / 100;" << endl <<
	"	}" << endl <<
	"	return factor;" << endl <<
	"}" << endl <<
	endl <<
	"function zoomIndependentScrollByLogicalPixels (dx, dy) {" << endl <<
	"	var zoomFactor = getZoomFactor();" << endl <<
	"	window.scrollBy (dx * zoomFactor, dy * zoomFactor);" << endl <<
	"}" << endl <<
	endl;
	 */
	
	
	headerStr << "<defs>" << endl;
	
	//headerStr << "<script type=\"text/ecmascript\" src=\"Scroll.js\">"
	//<< endl << "</script>" << endl;
	
	
	headerStr << "</defs>" << endl;
	return headerStr.str();
}


void Svg::addLine(
  unsigned int x1, unsigned int y1,
  unsigned int x2, unsigned int y2,
  unsigned int thickness,
  const string color,
	const string dashArray,
  const string title) {

  bodyStr_ << "<g fill='none' stroke='" << color
         <<"' stroke-width='" << thickness << "' ";
	if (dashArray != SOLID_DASH_ARRAY) {
	  bodyStr_ << "stroke-dasharray='" << dashArray << "'"; 
	}
	bodyStr_ << ">" << endl;
  bodyStr_ << "<line x1='" << x1 << "' y1='" << y1
    << "' x2='" << x2 << "' y2='" << y2 << "'>" << endl;	
	if (title!="") {
		bodyStr_ << "    <title>" << title << "</title>" << endl;
	}
	bodyStr_ << "</line>\n";
	
  bodyStr_ << "</g>\n";
}

void Svg::addCircle(
  unsigned int cx, unsigned int cy,
  unsigned int radius,
  unsigned int strokeWidth,
  const string strokeColor,
  const string fillColor) {

  bodyStr_ << "<circle cx='" << cx << "' cy='" << cy << "' r='" 
	<< radius << "'" << endl;
  bodyStr_ << "stroke='" << strokeColor
    << "' stroke-width='" << strokeWidth << "' fill='" << fillColor 
	<< "'/>" << endl;
}

void Svg::adaptUsedAreaToX(unsigned int x) {
	if (x < minX_) {
		minX_ = x;
	}
	if (x > maxX_) {
		maxX_ = x;
	}
}

void Svg::adaptUsedAreaToY(unsigned int y) {
	if (y < minY_) {
		minY_ = y;
	}
	if (y > maxY_) {
		maxY_ = y;
	}
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
   
	adaptUsedAreaToX(x);
	adaptUsedAreaToY(y);
	adaptUsedAreaToX(x+width);
	adaptUsedAreaToY(y+height);	
	
	const bool htmlTypeLink = false;
	const bool javascriptTypeLink = true;
	
  if (xlinkTo != "") {
		if (htmlTypeLink) {
			bodyStr_ << "<a xlink:href=\"" 
			<< fileName_ 
			<< "#" << xlinkTo 
			<< "\"" << ">" 
			<< endl;
		} else { // javascript type link
			bodyStr_ << "<a xlink:href=\"" 
			//<< fileName_ 
			//<< "#" << xlinkTo 
			//<< "javascript:window.scroll(300,400)"switchTransfer
			<< "javascript:jumpFromSrcIdToDstId('" << id << "', '" << xlinkTo << "')"
			<< "\"" << ">" 
			<< endl;
		}
	}
	bodyStr_
	<< "  <rect ";
	
	if (id!="") {
    bodyStr_ << "id=" << "\"" << id << "\"" << endl;
	}
	
	bodyStr_
	<< "x='" << x << "' y='" << y 
	<< "' width='" << width
	<< "' height='" << height
	<< "' style='fill:" << fillColor 
	<< ";stroke:" << strokeColor 
	<< ";stroke-width:" << strokeWidth << ";"
	<< "fill-opacity:" << fillOpacity 
	<< ";stroke-opacity:" << strokeOpacity << "'" << endl;
	if (javascriptTypeLink) {
		//bodyStr_ << "  onclick='javascript:window.scroll(300,400)'" << endl;
	}
	bodyStr_ << "> " << endl;
	
	if (title!="") {
		bodyStr_ << "    <title>" << title << "</title>" << endl;
	}
		
	bodyStr_	
	<< "  </rect>" << endl;	 

  if (xlinkTo != "") {
		//if (htmlTypeLink) {
		  bodyStr_ << "</a>" << endl;
		//} else { // javascript type link
		//}
	}
	bodyStr_ << endl;
}

void Svg::addText(
  unsigned int x, unsigned int y,
  const string & text,
  const string color,
  unsigned int fontSize,
  const string fontName,
  unsigned int angle) {

  bodyStr_ << "<g font-size='" << fontSize << "' font-family='" 
	<< fontName << "' >\n";
  bodyStr_ << "<text ";
  bodyStr_ << " style='stroke:none; fill:" << color << ";'";
  bodyStr_ << " x='" << x << "' y='" << y << "'";
  bodyStr_ << " transform='rotate(" << angle << ", " << x << ", " << y << ")'";
  bodyStr_ << ">";
  bodyStr_ << text << "</text></g>" << endl;
}

void Svg::addSvgString(const string & svgString) {
  bodyStr_ << svgString;
}

std::ostringstream & Svg::getBodyStream() {
  return bodyStr_;
}

std::string Svg::getBody() const {
	return bodyStr_.str();
}

string Svg::getFooter() const {
	stringstream footerStr;
  footerStr << "</svg>" << endl;
	return footerStr.str();
}

void Svg::cropToUsedArea() const {
  unsigned int newWidth  = maxX_ - minX_ + 1;
  unsigned int newHeight = maxY_ - minY_ + 1;
	
	width_  = newWidth  + 2 * CROP_BORDER;
	height_ = newHeight + 2 * CROP_BORDER;
}

string Svg::getTranslationHeader() const {
	stringstream strStr;
  strStr << "<g transform='translate(" << -(minX_ - CROP_BORDER) 
	  << "," << -(minY_-CROP_BORDER) << ")'" << ">" 
	  << endl;
	return strStr.str();
}

string Svg::getTranslationFooter() const {
	stringstream strStr;
	strStr << "</g>" << endl;
	return strStr.str();
}

void Svg::svgWrite(ostream & ostr) const {
	
	//ofStr << getHeader();
  //ofStr << bodyStr_.str();
	//ofStr << getFooter();
	
  //ofStr.close();
	
  cropToUsedArea(); // affects width_ and height_
  string header = getHeader();
	string transformHeader = getTranslationHeader();
	
	ostr << header;
	ostr << transformHeader;
// no grid here
  ostr << getBody(); // before body is written so it sits underneath body
	ostr << getTranslationFooter();
	ostr << getFooter();
}

void Svg::close() const {
  ofstream ofStr(fileName_.c_str());
	svgWrite(ofStr);
  ofStr.close();	
}

Svg::~Svg() {
}