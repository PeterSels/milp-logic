#include <fstream>
#include <assert.h>

#include "ScheduleSvg.h"
#include "StringUtilities.h"

using namespace std;

#define FONT_SIZE (8)
#define FONT_TYPE "Verdana"
#define GRID_COLOR "gray"

ScheduleSvg::ScheduleSvg(const string & fileName,
												 unsigned int width, 
												 unsigned int height,
												 unsigned int majorTickInterval,
												 unsigned int minorTickInterval,
												 string majorTickStrokeStyle,
												 string minorTickStrokeStyle)
: Svg(fileName, width, height) 
{
	addVerticalGridLines(majorTickInterval, 
											 minorTickInterval, 
											 majorTickStrokeStyle, 
											 minorTickStrokeStyle);
}

void ScheduleSvg::addVerticalGridLines(unsigned int majorTickInterval,
																			 unsigned int minorTickInterval,
																			 string majorTickStrokeStyle,
																			 string minorTickStrokeStyle) {
	if (majorTickInterval==0) { // means: no grid please
		return;
	}
  if (minorTickInterval==0) {
		minorTickInterval = majorTickInterval;
	}
	assert(minorTickInterval <= majorTickInterval);
	assert(majorTickInterval % minorTickInterval == 0);
	
  const unsigned int timeRowHeight = 20;
	unsigned int loTime = 0;
	unsigned int hiTime = width_; // supposedly 1 pixel per time unit
	//double sx = 1.0;
	unsigned int firstGridTime = 
	  loTime + minorTickInterval - (loTime % minorTickInterval);
	unsigned int lastGridTime = 
	  hiTime - (hiTime % minorTickInterval);
  for (unsigned int t=firstGridTime; t<=lastGridTime; t+=minorTickInterval) {
		bool major = t % majorTickInterval == 0;
		bool minor = t % minorTickInterval == 0;
		
		string strokeStyle = major ? majorTickStrokeStyle : minorTickStrokeStyle;

		if (major || minor) {
			stringstream timeStrStr;
			timeStrStr << "t=" << toTime_HH_MM(t);
			unsigned int x = (unsigned int)(t-loTime);
			// grey times text
			addText(x, timeRowHeight/2, 
							timeStrStr.str(), GRID_COLOR, FONT_SIZE, FONT_TYPE, 0);
			// vertical blue grid lines
			addLine(x, timeRowHeight/2,
							x, height_, 1, GRID_COLOR, strokeStyle, toTime_HH_MM(t));
		}
  }
}

void ScheduleSvg::addHorizontalGridLineAtY(unsigned int y, 
																					 const string & text) {
  // horizontal blue grid line
	addText(0, y-2, text, 
					GRID_COLOR, FONT_SIZE, FONT_TYPE, 0);
	addLine(0, y, width_-1, y, 1, GRID_COLOR);
}

ScheduleSvg::~ScheduleSvg() {
}
