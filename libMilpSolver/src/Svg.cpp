#include <fstream>
#include <assert.h>

#include "Svg.h"
#include "StringUtilities.h"

#define CROP_BORDER (1000)
#define HOVER_FILL_OPACITY (0.5)

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
, haveScrollJs_(false)
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
	
  headerStr << "<svg ";
	
	stringstream scrollJsStrStr;
	haveScrollJs_ = ifCanReadFromWriteTo("Scroll.js", scrollJsStrStr);
	if (haveScrollJs_) {
		headerStr << "onload='centerWindow()' ";
	}
	
	headerStr << "version='1.1' baseProfile=\"full\"" << endl;
	
	headerStr << "  xmlns='http://www.w3.org/2000/svg'" << endl;
	headerStr << "  xmlns:xlink='http://www.w3.org/1999/xlink'" << endl;
	headerStr << "  xmlns:ev='http://www.w3.org/2001/xml-events'" << endl;
	headerStr << "  width='" << width_ << "px'" << endl; 
	headerStr << "  height='" << height_ << "px'" << endl;
	headerStr << ">" << endl;
	
  headerStr << "<g><title>" << fileName_ << "</title></g>" << endl;
	
	if (haveScrollJs_) {
	  headerStr << "<script type=\"text/ecmascript\"><![CDATA[" << endl;
	  headerStr << scrollJsStrStr.str();
    headerStr << endl
    << "function makeTransparent(evt) {" << endl
    << "  evt.target.setAttributeNS(null,'opacity','" 
    << HOVER_FILL_OPACITY << "');" << endl
    << "}" << endl
    << endl
    << "function makeOpaque(evt) {" << endl
    << "  evt.target.setAttributeNS(null,'opacity','1.0');" << endl
    << "}" << endl
    << "function makeInvisible(evt) {" << endl
    //<< "  alert('in alert')" << endl
    //<< "  alert(getSvgViewerVersion())" << endl
    << "  evt.target.setAttributeNS(null,'opacity','0.0');" << endl
    << "}" << endl;
    
    headerStr
    << "function rectangleOverAction(evt) {" << endl
    << "  makeTransparent(evt)" << endl
    << "  //addHeaderObject(evt)" << endl
    << "  replaceHeaderObject(evt)" << endl
    << "}" << endl
    << endl
    << "function rectangleOutAction(evt) {" << endl
    << "  makeOpaque(evt);" << endl
    << "  //removeHeaderObject(evt);" << endl
    << "}" << endl
    << endl
    << "function makeTransparent(evt) {" << endl
    << "  evt.target.setAttributeNS(null,'opacity','0.5');" << endl
    << "}" << endl
    << "" << endl
    << "function makeOpaque(evt) {" << endl
    << "  evt.target.setAttributeNS(null,'opacity','1.0');" << endl
    << "}" << endl
    << "" << endl
    << "function addHeaderObject(evt) {" << endl
    << "  // Firefox needs .target io .srcElement" << endl
    << "  srcEl = evt.srcElement ? evt.srcElement : evt.target;" << endl
    << "  dataFile = srcEl.attributes.getNamedItem('data-file').value;" 
    << endl
    << "  // for window.top to work in Chrome, start it using" << endl
    << "  // /Applications/Google Chrome.app/Contents/MacOS/Google Chrome "
    << endl
    << "  // --disable-web-security" << endl
    << "  var doc = window.top.document;" << endl
    << "  var obj = doc.createElement('object', true);" << endl
    << "  obj.setAttribute('type', 'image/svg+xml');" << endl
    << "  obj.setAttribute('data', dataFile);" << endl
    << "  obj.setAttribute('width', '700');" << endl
    << "  obj.setAttribute('height', '700');" << endl
    << "  obj.setAttribute('id', 'headerObject');" << endl
    << "  obj.addEventListener('load', function() {" << endl
    << "    // alert('loaded!');" << endl
    << "  }, false);" << endl
    << "  if (doc.body) { // in case of running svg without html wrapper" 
    << endl
    << "      doc.body.appendChild(obj);" << endl
    << "    }" << endl
    << "}" << endl
    << endl
    << "function removeHeaderObject(evt) {" << endl
    << "  var doc = window.top.document;" << endl
    << "  var childId = doc.getElementById('headerObject');" << endl
    << "  if (childId) { // to avoid error in first time case" << endl
    << "    if (doc.body) { // in case of running svg without html wrapper" 
    << endl
    << "      doc.body.removeChild(childId);" << endl
    << "    }" << endl
    << "  }" << endl
    << "}" << endl
    << "function replaceHeaderObject(evt) {" << endl
    << "  removeHeaderObject(evt);" << endl
    << "  addHeaderObject(evt);" << endl
    << "}" << endl    
    
    << endl;    
    
	  headerStr << "]]></script>" << endl;
	}

	headerStr << "<defs>" << endl;
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
	
	adaptUsedAreaToX(x1);
	adaptUsedAreaToX(x2);
	adaptUsedAreaToY(y1);
	adaptUsedAreaToY(y2);
	
  bodyStr_ << "<g fill='none' stroke='" << color
         <<"' stroke-width='" << thickness << "' ";
	if (dashArray != SOLID_DASH_ARRAY) {
	  bodyStr_ << "stroke-dasharray='" << dashArray << "'"; 
	}
	bodyStr_ << ">" << endl;
  bodyStr_ << "<line ";
	
	// For Firefox only:
	bodyStr_ << "xlink:title=\"" << title << "\" " << endl;

	bodyStr_ << "x1='" << x1 << "' y1='" << y1
    << "' x2='" << x2 << "' y2='" << y2 << "'>" << endl;	
	if (title!="") {
		bodyStr_ << "    <title>" << title << "</title>" << endl;
	}
	bodyStr_ << "</line>" << endl;
	
  bodyStr_ << "</g>" << endl;
}

void Svg::addCircle(
  unsigned int cx, unsigned int cy,
  unsigned int radius,
  unsigned int strokeWidth,
  const string strokeColor,
  const string fillColor) {

	adaptUsedAreaToX(cx+radius);
	adaptUsedAreaToX(cx-radius);
	adaptUsedAreaToY(cy+radius);
	adaptUsedAreaToY(cy-radius);
		
  bodyStr_ << "<circle cx='" << cx << "' cy='" << cy << "' r='" 
	<< radius << "'" << endl;
  bodyStr_ << "stroke='" << strokeColor
    << "' stroke-width='" << strokeWidth << "' fill='" << fillColor 
	<< "'/>" << endl;
}

void Svg::adaptUsedAreaToX(int x) {
	if (x < 0) {
		x = 0;
	}
	if (x < minX_) {
		minX_ = x;
	}
	if (x > maxX_) {
		maxX_ = x;
	}
}

void Svg::adaptUsedAreaToY(int y) {
	if (y < 0) {
		y = 0;
	}
	if (y < minY_) {
		minY_ = y;
	}
	if (y > maxY_) {
		maxY_ = y;
	}
}

void Svg::addRectangle(unsigned int xTmp,
											 unsigned int y,
											          int widthTmp, 
											 unsigned int height,
											 unsigned int strokeWidth,
											 const string strokeColor,
											 const string fillColor,
											 float strokeOpacity,
											 float fillOpacity,
											 string title,
											 string id,
                       string dataFile,
											 string xlinkTo) {
	
	// default
	unsigned int x = xTmp;
	unsigned int width = widthTmp;
	// but
	if (widthTmp < 0) {
		int xHi = xTmp;
		int xLo = xTmp + widthTmp;
		assert(xLo <= xHi);
		x = xLo;
		width = (unsigned)(-widthTmp);
		assert((int)x + (int)width == xHi);
	}
	
	adaptUsedAreaToX(x);
	adaptUsedAreaToY(y);
	adaptUsedAreaToX(x+width);
	adaptUsedAreaToY(y+height);
	
	const bool htmlTypeLink = false;
	const bool javascriptTypeLink = true;
	assert(!htmlTypeLink || !javascriptTypeLink);
	
  if (xlinkTo != "") {
		if (htmlTypeLink) {
			bodyStr_ << "<a xlink:href=\"" 
			<< fileName_ 
			<< "#" << xlinkTo 
			<< "\"" << ">" 
			<< endl;
		}
		
		if (javascriptTypeLink) {
			bodyStr_ << "<a xlink:href=\"" 
			<< "javascript:jumpFromSrcIdToDstId('" << id << "', '" << xlinkTo << "')"
			<< "\"" << ">" 
			<< endl;
		}
	}
	bodyStr_ << "  <rect ";
	
	// For Firefox only:
	bodyStr_ << "xlink:title=\"" << title << "\" " << endl;
	
	if (id!="") {
    bodyStr_ << "id=" << "\"" << id << "\"" << endl;
	}
	
	bodyStr_
	<< "x='" << x << "' y='" << y 
	<< "' width='" << width
	<< "' height='" << height
	<< "' style='fill:" << fillColor 
	<< ";stroke:" << strokeColor 
	<< ";stroke-width:" << strokeWidth
	<< ";fill-opacity:" << fillOpacity 
	<< ";stroke-opacity:" << strokeOpacity << "'" << endl;
  
	bodyStr_
  //<< "onload='makeInvisible(evt)' " << endl
  //<< " onmouseover='makeInvisible(evt)'"
  
  << " onmouseover='rectangleOverAction(evt)'"
  << " onmouseout='rectangleOutAction(evt)'" 
  << " data-file='" << dataFile << "'" // won't xhtml validate, but ok
  
  //<< " onmouseover='showImage(\"" << dataFile << "\", 0, 500, 1000, 600); changeColor(blue)'"
  << endl;
	bodyStr_ << "> " << endl;
	
	if (title!="") {
		bodyStr_ << "    <title>" << title << "</title>" << endl;
		//bodyStr_ << "    <desc>'" << dataFile << "'</desc>" << endl;
	}
  
	bodyStr_	
	<< "  </rect>" << endl;	 

  if (xlinkTo != "") {
		if (javascriptTypeLink) {
		  bodyStr_ << "</a>" << endl;
		}
		if (htmlTypeLink) {
		  bodyStr_ << "</a>" << endl;
		}
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

	// angle independent for now:
	adaptUsedAreaToX(x - fontSize * text.length());
	adaptUsedAreaToX(x + fontSize * text.length());
	adaptUsedAreaToY(y - fontSize * text.length());
	adaptUsedAreaToY(y + fontSize * text.length());
	
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
  strStr << "<g transform='translate(" << -((int)minX_ - CROP_BORDER) 
	  << "," << -((int)minY_-CROP_BORDER) << ")'" << ">" 
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


void Svg::svgHtmlWrapperWrite(ostream & ostr) const {
  
  const bool header = true;
  const bool footer = false;
  /*
  ostr <<
  "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
  "<!--DVID=0002EED6-->" << endl;
  //"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 4.01 Frameset//EN\" \"http://www.w3.org/TR/html4/frameset.dtd\">"
  //<< endl;
/*
  ostr <<
  "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
  "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
  "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\">"
  << endl;
*/
  
  ostr << 
  "<html>" << endl
  << endl
  << "<head>" << endl
  << endl;
  
  // CSS for z index
  ostr << "<style type=\"text/css\">" << endl;
  ostr << endl;
  
  ostr << "div#CENTER {" << endl;
  //ostr << "  background-color: #000000;" << endl;
  ostr << "  z-index: 1;" << endl;
  //ostr << "  position: absolute;" << endl;
  ostr << "}" << endl;
  
  ostr << "div#HEADER {" << endl;
  //ostr << "  background-color: #333333;" << endl;
  ostr << "  z-index: 2;" << endl;
  //ostr << "  position: absolute;" << endl;
  ostr << "}" << endl;
  ostr << endl;
  
  ostr << "</style>" << endl;
  ostr << endl;
  
  ostr
  << "<script>" << endl
  << "window.onload=prepare" << endl
  << endl
  << "function prepare() {" << endl
  << "if (document.body.clientWidth) {" << endl
  << "  w  = document.body.clientWidth" << endl
  << "  h = document.body.clientHeight" << endl
  //<< "headerH = document.getElementById('HEADER').clientHeight" << endl
  //<< "footerH = document.getElementById('FOOTER').clientHeight" << endl
  << "} else {" << endl
  << "  w = window.innerWidth" << endl
  << "  h = window.innerHeight" << endl
  //<< "headerH = document.getElementById('HEADER').innerHeight" << endl
  //<< "footerH = document.getElementById('FOOTER').innerHeight" << endl
  << "}" << endl;
  

  if (header) {
    ostr << "headerH = 700" << endl;
  } else {
    ostr << "headerH = 0" << endl;
  }
  if (footer) {
    ostr << "footerH = 70" << endl;
  } else {
    ostr << "footerH = 0" << endl;
  }
  ostr << "restH = 0;" << endl;
  

  
  ostr
  << "document.getElementById('CENTER').setAttribute('width', w)" << endl
  << "document.getElementById('CENTER')"
  //<< ".setAttribute('height', h - headerH - footerH)" << endl
  << ".setAttribute('height', h)" << endl
  //<< "document.getElementById('CENTER').setAttribute('z-index', 1)" << endl
  << "document.getElementById('CENTER').style.left = 0" << endl
  //<< "document.getElementById('CENTER').style.top  = headerH + restH" << endl
  << "document.getElementById('CENTER').style.top  = 0" << endl
  << endl;
  
  
  if (header) {
    ostr
    << "document.getElementById('HEADER').setAttribute('width', w)" << endl
    //<< "document.getElementById('HEADER').setAttribute('height', headerH)" 
    << "document.getElementById('HEADER').setAttribute('height', h)" 
    << endl
    //<< "document.getElementById('HEADER').setAttribute('z-index', 2)" << endl
    << "document.getElementById('HEADER').style.left = 0" << endl
    << "document.getElementById('HEADER').style.top  = 0" << endl
    << endl;
  }
  
  if (footer) {    
    ostr
    << "document.getElementById('FOOTER').setAttribute('width', w)" << endl
    << "document.getElementById('FOOTER').setAttribute('height', footerH)" 
    << endl
    << "document.getElementById('FOOTER').setAttribute('height', footerW)" 
    << endl
    << "document.getElementById('FOOTER').style.left = 0" << endl
    << "document.getElementById('FOOTER').style.top  = h - footerH" << endl
    << endl;
  }
  ostr
  << "}" << endl;
    

  
  ostr
  << "</script>" << endl
  << endl
  << "</head>" << endl
  << endl
  << "<body>" << endl
  << endl;
  
  


  ostr
  << "<div>" << endl
  << "<embed id='CENTER' src='" << fileName_ << "' " << endl
  //<< "style='position:absolute; left:0;top:800'>" << endl
  << "style='position:absolute; left:0;top:0'>" << endl
  << "</div>" << endl
  << endl;
  
  
  if (header) {
    ostr
    //<< "<div id='HEADER' style='position:absolute;top:0' clientHeight=700>" 
    << "<div id='HEADER' style='position:absolute;top:0'>" 
    << endl
    //<< "<p>Schedule Generated by RhinoCeros</p>" << endl
    //<< "<hr>" << endl
    << "</div>" << endl
    << endl;
  }
  
  if (footer) {
    ostr
    << "<div id='FOOTER' style='position:absolute;top:1400' clientHeight=70>" 
    << endl
    << "<hr>" << endl
    << "<p>sels.peter@gmail.com</p>" << endl
    << "</div>" << endl
    << endl;
  }
  
  ostr
  << "</body>" << endl
  << endl
  << "</html>" << endl
  << endl;
}

void Svg::close() const {
  ofstream ofStr(fileName_.c_str());
	svgWrite(ofStr);
  ofStr.close();  
}

Svg::~Svg() {
}
