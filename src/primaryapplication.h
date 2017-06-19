/*

Eric Wolfson
primaryapplication.h
Class declaration for primary window

*/

#ifndef PRIMARYAPPLICATION_H_
#define PRIMARYAPPLICATION_H_

#include "executioncontrolbox.h"
#include "analysischart.h"
#include "inputcontrolgroup.h"

void initProgData();

// Main class of entire program.
class primaryApplicationClass : public Fl_Double_Window {
   public:
      primaryApplicationClass(int,int);
   private:
      executionControlBox *ecb_obj;
      inputControlGroup *icg_obj;
      analysisChart *chart_obj;
      Fl_Hor_Slider *chart_slider;
      Fl_Scroll *chart_scroller;
      Fl_Button *chart_refresh_button;
      Fl_Box *chart_label;
      // legend for chart
      Fl_Box *running_color_indicator;
      Fl_Box *waiting_color_indicator;
      Fl_Box *deadlock_color_indicator;
      Fl_Box *text_indicator1;
      Fl_Box *text_indicator2;
      Fl_Box *text_indicator3;
};

#endif
