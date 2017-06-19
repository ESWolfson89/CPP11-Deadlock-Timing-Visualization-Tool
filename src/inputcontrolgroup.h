/*

Eric Wolfson
inputcontrolgroup.h
Class for input table object and relevant widgets

*/


#ifndef INPUTCONTROLGROUP_H_
#define INPUTCONTROLGROUP_H_

#include "inputtable.h"

// Class containing:
// 1) dataTable widget
// 2) 7 random input parameter fields
// 3) File browser with refresh button
// All 3 of these need to communicate together,
// so they are grouped together.
class inputControlGroup {
   public:
      inputControlGroup();
      inputControlGroup(int,int,int,int,int,int);
      void initCurrentInputParametersIntegerInputs();
      void initParameterInputFilesBrowser();
      void initFileRefreshButton();
      dataTable *getDataTable();
      Fl_Int_Input *getCurrentInputParameterIInp(int);
   private:
      dataTable *table_obj;
      Fl_Int_Input *current_input_parameters_iinp[NUM_INPUT_PARAMETER_TYPES];
      Fl_Button *input_file_refresh_button;
      Fl_File_Browser *input_parameters_fb;
      Fl_Box *tabel_label;
};

//callbacks
void inputFileRefreshButton_CB(Fl_Widget *, void *);
void selectInputFile_CB(Fl_Widget *, void *);
void programDurationIInp_CB(Fl_Widget *, void *);
void numMutexesIInp_CB(Fl_Widget *, void *);
void numThreadsIInp_CB(Fl_Widget *, void *);
void minSleepValIInp_CB(Fl_Widget *, void *);
void maxSleepValIInp_CB(Fl_Widget *, void *);
void numThreadInstructionPairsIInp_CB(Fl_Widget *, void *);
void lockChanceIInp_CB(Fl_Widget *, void *);

// helpers
void setupFileBrowser(Fl_File_Browser *);
void loadInputFileToGrid(inputControlGroup *);

#endif
