/*

Eric Wolfson
progressbar.cpp
Indicates execution pipelines progress via colored blue bar

*/

#include "progressbar.h"

executionProgressBar::executionProgressBar(int x, int y, int w, int h) : Fl_Box(x,y,w,h,0) {
   progress_ratio = 0.0;
}

void executionProgressBar::setProgressRatio(double pr) {
   progress_ratio = pr;
}

// Override of Fl_Widget's virtual draw function.
// Draws a light blue progress bar.
// Length depends on pipeline progress, and represents a percentage.
void executionProgressBar::draw() {
   int back_color = FL_BACKGROUND_COLOR;
   int fore_color = 0x7f7fff00;
   fl_draw_box(FL_SHADOW_BOX,x(),y(),w(),h(),back_color);
   fl_draw_box(FL_SHADOW_BOX,x(),y(),(int)(((double)w())*progress_ratio),h(),fore_color);
   fl_font(FL_HELVETICA,12);
   fl_draw(label(),x(),y(),w(),h(),FL_ALIGN_CENTER,NULL,0);
}
