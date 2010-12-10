#include <fstream>
#include <assert.h>

#include "ScheduleSvg.h"
#include "StringUtilities.h"

using namespace std;

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
	setGridParameters(majorTickInterval, 
										minorTickInterval, 
										majorTickStrokeStyle, 
										minorTickStrokeStyle);
}
void ScheduleSvg::setGridParameters(unsigned int majorTickInterval,
																		unsigned int minorTickInterval,
																		string majorTickStrokeStyle,
																		string minorTickStrokeStyle) {
	// store:
	majorTickInterval_    = majorTickInterval;
	minorTickInterval_    = minorTickInterval;
	majorTickStrokeStyle_ = majorTickStrokeStyle;
	minorTickStrokeStyle_ = minorTickStrokeStyle;
	
  // range and compatibility checks:	
	if (minorTickInterval_==0) {
		minorTickInterval_ = majorTickInterval_;
	}
	assert(minorTickInterval_ <= majorTickInterval_);
	assert(majorTickInterval_ % minorTickInterval_ == 0);
}

void ScheduleSvg::createGrid() const {
	if (majorTickInterval_==0) { // means: no grid please
		return;
	}
	
	Svg dummySvg("ScheduleSvg__createGridDummy.svg", width_, height_);
	// is never closed, so never written to file
	
  const unsigned int timeRowHeight = 20;
	unsigned int loTime = 0;
	unsigned int hiTime = width_; // supposedly 1 pixel per time unit
	unsigned int firstGridTime = 
	  loTime + minorTickInterval_ - (loTime % minorTickInterval_);
	unsigned int lastGridTime = 
	  hiTime - (hiTime % minorTickInterval_);
  for (unsigned int t=firstGridTime; t<=lastGridTime; t+=minorTickInterval_) {
		bool major = t % majorTickInterval_ == 0;
		bool minor = t % minorTickInterval_ == 0;
		
		string strokeStyle = major ? majorTickStrokeStyle_ : minorTickStrokeStyle_;

		if (major || minor) {
			stringstream timeStrStr;
			timeStrStr << "t=" << toTime_HH_MM(t);
			unsigned int x = (unsigned int)(t-loTime);
			// grey times text
			dummySvg.addText(x, timeRowHeight/2, 
							timeStrStr.str(), GRID_COLOR, FONT_SIZE, FONT_TYPE, 0);
			// vertical blue grid lines
			dummySvg.addLine(x, timeRowHeight/2,
							x, height_, 1, GRID_COLOR, strokeStyle, toTime_HH_MM(t));
		}
  }
	gridStr_ << dummySvg.getBody();
}

std::string ScheduleSvg::getGrid() const {
	return gridStr_.str();
}

void ScheduleSvg::addHorizontalGridLineAtY(unsigned int y, 
																					 const string & text) {
  // horizontal blue grid line
	addText(0, y-2, text, 
					GRID_COLOR, FONT_SIZE, FONT_TYPE, 0);
	addLine(0, y, width_-1, y, 1, GRID_COLOR);
}

void ScheduleSvg::close() const {
	createGrid(); // at last, after final width_ and height_ are set
	string grid = getGrid();
	
  cropToUsedArea();
  string header = getHeader();
	string transformHeader = getTranslationHeader();
	
  ofstream ofStr(fileName_.c_str());
	
	ofStr << header;
	ofStr << transformHeader;
	ofStr << grid; // grid has to be written 
  ofStr << getBody(); // before body is written so it sits underneath body
	ofStr << getTranslationFooter();
	ofStr << getFooter();
	
  ofStr.close();
}

ScheduleSvg::~ScheduleSvg() {
}
