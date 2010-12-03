#ifndef SVG_H
#define SVG_H

#include <sstream>

class Svg {
public:
	Svg(const std::string & fileName,
		unsigned int width, unsigned int height);
	void addLine(
    unsigned int x1, unsigned int y1,
	  unsigned int x2, unsigned int y2,
	  unsigned int thickness = 1,
	  const std::string color = "black");
	
	void addCircle(
	  unsigned int cx, unsigned int cy,
	  unsigned int radius = 1,
	  unsigned int strokeWidth = 1,
	  const std::string strokeColor = "black",
	  const std::string fillColor = "black");
	
	void addRectangle(unsigned int x, unsigned int y,
										unsigned int width, unsigned int height,
										unsigned int strokeWidth,
										const std::string & strokeColor,
										const std::string & fillColor,
										float strokeOpacity=1.0,
										float fillOpacity=1.0);	
	void addText(
	  unsigned int x, unsigned int y,
	  const std::string & text,
	  const std::string color = "black",
	  unsigned int fontSize = 10,
	  const std::string fontName = "Verdana",
	  unsigned int angle = 0);
  void addSvgString(const std::string & svgString);
  std::ostringstream & getStream();
	void close(); // also writes to disk
	~Svg();
protected:
	void addHeader();
	void addFooter();
private:
  std::ostringstream strstr_;
  std::string fileName_;
  unsigned int width_;
  unsigned int height_;
};


#endif // SVG_H
