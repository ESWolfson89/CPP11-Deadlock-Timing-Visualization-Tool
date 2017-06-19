/*

Eric Wolfson
analysischart.h
The primary chart visualization (declaration)

*/

#ifndef ANALYSISCHART_H_
#define ANALYSISCHART_H_

#include "processdeadlock.h"
#include "fltkincludes.h"

// A segment in a thread bar in the display chart widget. 
class barSegment {
   public:
      barSegment();
      barSegment(int,int,int,std::string,std::string);
      int getStartX();
      int getEndX();
      int getWidth();
      void setStartX(int);
      void setEndX(int);
      void setColor(int);
      void setStartString(std::string);
      void setEndString(std::string);
      int getColor();
      std::string getStartString();
      std::string getEndString();
   private:
      // Is defined by its start point, end point (along x axis) and color
      int startx;
      int endx;
      int color;
      // Needed for displaying event that occurred at start (and possibly end) of segment
      std::string start_str;
      std::string end_str;
};

// Set of segments for a thread
// Represents activity of thread (not including unlocks, which are stored in a different object)
class threadActivityBar {
   public:
      threadActivityBar();
      void addSegment(int,int,int,std::string,std::string);
      void setSegmentStartX(int,int);
      void setSegmentEndX(int,int);
      void setSegmentColor(int,int);
      void setSegmentStartString(int,std::string);
      void setSegmentEndString(int,std::string);
      void eraseSegment(int);
      int getSegmentStartX(int);
      int getSegmentEndX(int);
      int getSegmentWidth(int);
      int getNumSegments();
      int getSegmentColor(int);
      std::string getSegmentStartString(int);
      std::string getSegmentEndString(int);
   private:
      std::vector<barSegment> segments;
};

// Contains unlock display data:
// 1) Position along the display widget,
// and 3) Indicator string to be displayed on the chart indicating
// mutex or range of mutexes unlocked in the vicinity.
struct unlock_display_type {
   int position;
   int mut_val;
   std::string unlock_str;
};

// I use a struct here to avoid using a 3D vector.
struct chart_data {
   std::vector<threadActivityBar> thread_bars;
   std::vector< std::vector<unlock_display_type> > thread_bar_unlock_data;
};

// The large display chart that takes up a large portion of the window
class analysisChart : public Fl_Box {
   public:
      analysisChart(int,int,int,int);
      // For the next 2 "populate" functions:
      // Store data needed such as color and text data on chart...
      // Once virtual draw function is called, this data is used.
      void populateThreadActivityBarVector(int);
      void populateThreadBarUnlockPositionsVector(int);
      void setTempThreadQuantity(int);
      void setScroller(Fl_Scroll *);
      void setSlider(Fl_Hor_Slider *);
      void setInternalViewButton(Fl_Button *);
      void setChartHeight(int);
      void resizeThreadChartsVector(int);
      chart_data *getThreadChart(int);
      int getNumThreadCharts();
      int getCurrentXCursor();
      int getCurrentYCursor();
      void cleanUpChartData();
      void draw();
      Fl_Hor_Slider *getSlider();
      int handle(int);
   private:
      // Each chart_data object represents a single run through
      // the pipeline. Each object therefore is based on the same
      // input file.
      std::vector<chart_data> thread_charts;
      std::string cursor_str;
      // Memory allocated outside of class, but this class contains
      // instances of the scroller (for scrolling along the chart) and slider
      // (for iterating through "thread_charts", since data from the slider
      // and scroller is needed internally.
      // We need to call begin() (for scroller) and end() outside of implementation
      // for chart to display properly, since the chart_data is not actually inheriting
      // from any widget class, but the analysisChart class is. This is a subtle trick
      // that can be used with FLTK.
      Fl_Scroll *internal_scroller;
      Fl_Hor_Slider *internal_slider;
      int temp_thread_quantity;
      int bar_height;
      int currentx_cursor;
      int currenty_cursor;
      bool show_tooltip;
};

// Various other helper/UI functions
bool deadlockOccurredAtThisPoint(int,int,int,int);
std::string getDLStringOfThread(int,int,int,int);

void analysisChartLoadButton_CB(Fl_Widget *, void *);
void analysisChartSlider_CB(Fl_Widget *, void *);

#endif
