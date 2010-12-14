#ifndef SVG_H
#define SVG_H

#include <sstream>

#define SOLID_DASH_ARRAY ""
#define SHORT_DASH_ARRAY "4, 2, 4, 2"
#define LONG_DASH_ARRAY "8, 2, 8, 2"
#define MIXED_DASH_ARRAY "8, 2, 2, 2"

#define FONT_SIZE (8)
#define FONT_TYPE "Verdana"
#define DEFAULT_COLOR "black"

class Svg {
public:
	Svg(const std::string & fileName,
		unsigned int width, unsigned int height);

	// As long as you call these before close, even multiple times, 
	// it will have the wanted effect on your svg file:
	void setWidth(unsigned int width);
	void setHeight(unsigned int height);
	
	
	void addLine(
    unsigned int x1, unsigned int y1,
	  unsigned int x2, unsigned int y2,
	  unsigned int thickness = 1,
	  const std::string color = DEFAULT_COLOR,
		const std::string dashArray = SOLID_DASH_ARRAY,
    const std::string title = "");
	
	void addCircle(
	  unsigned int cx, unsigned int cy,
	  unsigned int radius = 1,
	  unsigned int strokeWidth = 1,
	  const std::string strokeColor = DEFAULT_COLOR,
	  const std::string fillColor = DEFAULT_COLOR);
	
	void adaptUsedAreaToX(int x);
	void adaptUsedAreaToY(int y);
	
	void addRectangle(unsigned int x, unsigned int y,
										unsigned int width, unsigned int height,
										unsigned int strokeWidth=1,
										const std::string strokeColor = DEFAULT_COLOR,
										const std::string fillColor = DEFAULT_COLOR,
										float strokeOpacity=1.0,
										float fillOpacity=1.0,
										std::string title="",
										std::string id="",
										std::string xlinkTo="");	
	void addText(
	  unsigned int x, unsigned int y,
	  const std::string & text,
	  const std::string color = DEFAULT_COLOR,
	  unsigned int fontSize = FONT_SIZE,
	  const std::string fontName = FONT_TYPE,
	  unsigned int angle = 0);
  void addSvgString(const std::string & svgString);
	std::string getHeader() const;
	std::string getTranslationHeader() const;
  std::ostringstream & getBodyStream();
	std::string getBody() const;
	std::string getTranslationFooter() const;
	std::string getFooter() const;
	
	void cropToUsedArea() const;
	void svgWrite(std::ostream & ostr) const;
	virtual void close() const; // also writes to file
	~Svg();
protected:	
  std::string fileName_;
  mutable unsigned int width_; // mutable for crop
  mutable unsigned int height_;
	
	int minX_;
	int maxX_;
	int minY_;
	int maxY_;
	
private:
  std::ostringstream bodyStr_;
	mutable bool haveScrollJs_;
};


#endif // SVG_H
