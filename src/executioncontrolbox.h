/*

Eric Wolfson
executioncontrolbox.h
Controls various inputs into the program GUI

*/

#ifndef EXECUTIONCONTROLBOX_H_
#define EXECUTIONCONTROLBOX_H_

#include "progressbar.h"
#include "cppgenerator.h"
#include "resultsanalyzer.h"
#include "processdeadlock.h"

// A widget containing a series of sub-widgets including:
// 1) Green execution button
// 2) pd_obj.ci_flag setter radio button
// 3) pd_obj.ri_flag setter radio button
// 4) Number of runs integer input for next set of runs (capped at the moment to 25)
// 5) Progress bar indicator widget box
class executionControlBox : public Fl_Box {
   public:
      executionControlBox(int,int,int,int);
      void initExecutionProgressIndicator();
      void initPrimaryExecutionButton();
      void initCreateInputRadioButton();
      void initRandomInputRadioButton();
      void initNumExecutionRunsIntegerInput();
   private:
      Fl_Button *primary_execution_button;
      Fl_Radio_Round_Button *create_input_radiobutton;
      Fl_Radio_Round_Button *random_input_radiobutton;
      Fl_Int_Input *num_execution_runs_iinp;
      Fl_Box *controlbox_label;
      executionProgressBar *execution_progress_indicator;
};

// callbacks pertaining to this widget
void primaryExecutionButton_CB(Fl_Widget *, void *);
void createInputRadioButton_CB(Fl_Widget *, void *);
void randomInputRadioButton_CB(Fl_Widget *, void *);
void numExecutionRunsIInp_CB(Fl_Widget *, void *);

// helpers
void reDisplayLabel(executionProgressBar *, std::string, int, int);
void populateDisplayGraphsVector();
void populateThreadBarPositionsVector();

#endif
