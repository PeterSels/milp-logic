#ifndef SCHEDULE_SVG_H
#define SCHEDULE_SVG_H

#include <sstream>
#include <string>

#include "Svg.h"
#include "NUnits.h"

class ScheduleSvg : public Svg {
public:
	ScheduleSvg(const std::string & fileName,
							unsigned int width, 
							unsigned int height,
							unsigned int majorTickInterval =      UU_PER_HR,
							unsigned int minorTickInterval = 15 * UU_PER_MIN,
							std::string majorTickStrokeStyle = SOLID_DASH_ARRAY,
							std::string minorTickStrokeStyle = SHORT_DASH_ARRAY);
	void setGridParameters(unsigned int majorTickInterval =      UU_PER_HR,
												 unsigned int minorTickInterval = 15 * UU_PER_MIN,
												 std::string majorTickStrokeStyle 
												 = SOLID_DASH_ARRAY,
												 std::string minorTickStrokeStyle 
												 = SHORT_DASH_ARRAY);	
	void createGrid() const;
	std::string getGrid() const;
	void addHorizontalGridLineAtY(unsigned int y, 
																const std::string & text);
	void close() const;
	~ScheduleSvg();
private:
	mutable std::stringstream gridStr_;
	unsigned int majorTickInterval_;
	unsigned int minorTickInterval_;
	std::string majorTickStrokeStyle_;
	std::string minorTickStrokeStyle_;
};

#endif // SCHEDULE_SVG_H
