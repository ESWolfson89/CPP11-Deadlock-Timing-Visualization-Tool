/*

Eric Wolfson
inputtable.h
Class definition (and callbacks) for instruction input table widget

*/

#ifndef INPUTTABLE_H_
#define INPUTTABLE_H_

#include "globalprogdata.h"
#include "fltkincludes.h"

// Represents Data input table
// for showing what pattern file is selecting as well
// as manually creating a pattern file.
class dataTable : public Fl_Table {
   public:
      dataTable(int,int,int,int,int,int);
      void draw_cell(TableContext,int,int,int,int,int,int);
      void resizeInstructionStringVector(int,int);
      void setInstructionString(int,int,std::string);
      void setInstructionStringThenHideInput();
      void checkUpdateGridSize();
      void resetEdit();
      void beginEdit(TableContext,int,int);
      bool isValidInstructionString(std::string &);
   private:
      Fl_Input *current_inp;
      TableContext edit_area;
      int in_row;
      int in_column;
};

//callbacks
void currentDataTableInput_CB(Fl_Widget *, void *);
void dataTable_CB(Fl_Widget *, void *);

#endif
