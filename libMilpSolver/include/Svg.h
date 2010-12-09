#ifndef SVG_H
#define SVG_H

#include <sstream>

#define SOLID_DASH_ARRAY ""
#define SHORT_DASH_ARRAY "4, 2, 4, 2"
#define LONG_DASH_ARRAY "8, 2, 8, 2"
#define MIXED_DASH_ARRAY "8, 2, 2, 2"

class Svg {
public:
	Svg(const std::string & fileName,
		unsigned int width, unsigned int height);
	void addLine(
    unsigned int x1, unsigned int y1,
	  unsigned int x2, unsigned int y2,
	  unsigned int thickness = 1,
	  const std::string color = "black",
		const std::string dashArray = SOLID_DASH_ARRAY,
    const std::string title = "");
	
	void addCircle(
	  unsigned int cx, unsigned int cy,
	  unsigned int radius = 1,
	  unsigned int strokeWidth = 1,
	  const std::string strokeColor = "black",
	  const std::string fillColor = "black");
	
	void addRectangle(unsigned int x, unsigned int y,
										unsigned int width, unsigned int height,
										unsigned int strokeWidth=1,
										const std::string strokeColor="black",
										const std::string fillColor="black",
										float strokeOpacity=1.0,
										float fillOpacity=1.0,
										std::string title="",
										std::string id="",
										std::string xlinkTo="");	
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
  std::string fileName_;
protected:
  unsigned int width_;
  unsigned int height_;
	private:
  std::ostringstream strstr_;
};


#endif // SVG_H
