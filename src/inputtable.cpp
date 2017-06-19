/*

Eric Wolfson
inputtable.cpp
Class implementation (and callbacks) for instruction input table widget

*/

#include "inputtable.h"

dataTable::dataTable(int x, int y, int w, int h, int r, int c) : Fl_Table(x,y,w,h,0) {
   callback(dataTable_CB);
   when(FL_WHEN_NOT_CHANGED|when());
   current_inp = new Fl_Input(0,0,0,0);
   current_inp->hide();
   current_inp->callback(currentDataTableInput_CB,(void *)this);
   current_inp->when(FL_WHEN_ENTER_KEY_ALWAYS|current_inp->when());
   current_inp->maximum_size(9);
   current_inp->box(FL_THIN_UP_BOX);
   end();
   in_row = in_column = 0;
   edit_area = CONTEXT_NONE;
   rows(pd_obj.parameter_input_data[(int)IPT_NUM_THREADS]);
   cols(c);
   row_height_all(20);
   col_width_all(50);
   resizeInstructionStringVector(cols(),rows());
}

// Override of virtual draw_cell function that draws a cell in the table.
// Each cell is either empty or shows a valid instruction string.
void dataTable::draw_cell(TableContext context, int r, int c, int x, int y, int w, int h) {
   if (context == CONTEXT_CELL) {
       if (edit_area != context || !current_inp->visible() || r != in_row || c != in_column) {
          fl_draw_box(FL_THIN_UP_BOX,x,y,w,h,0xdfdfdf00);
          fl_push_clip(x,y,w,h);
          fl_color(FL_BLACK);
          fl_font(FL_HELVETICA,14);
          fl_draw(&(pd_obj.instruction_strings[r][c])[0], x,y,w,h, FL_ALIGN_CENTER);
          fl_pop_clip();
       }
   }
   else if ((int)context == 1 || (int)context == 2) {
       int sx,sy,sw,sh;
       find_cell(CONTEXT_CELL,in_row,in_column,sx,sy,sw,sh);
       current_inp->resize(sx,sy,sw,sh);
   }
}

// The instruction string in the cells of the data table must
// be of the following format:
// (S or U or L or s or u or l) NUM
// where NUM is a number within certain predefined limits
bool dataTable::isValidInstructionString(std::string &inst_str) {
   if ((int)inst_str.size() < 2) {
       return false;
   }

   if (inst_str[0] == 's' || inst_str[0] == 'l' || inst_str[0] == 'u') {
       inst_str[0] = (char)((int)inst_str[0] - 32);
   }

   if (inst_str[0] != 'S' && inst_str[0] != 'L' && inst_str[0] != 'U') {
       return false;
   }

   for (int i = 1; i < (int)inst_str.size(); ++i) {
       if ((int)inst_str[i] < (int)'0' || (int)inst_str[i] > (int)'9') {
           return false;
       }
   }

   int num = std::stoi(inst_str.substr(1));
   
   if (inst_str[0] == 'S') {
       if (num > max_input_max_sleepval || num < min_input_min_sleepval) {
           return false;
       }
   }

   if (inst_str[0] == 'L' || inst_str[0] == 'U') {
       if (num > pd_obj.parameter_input_data[(int)IPT_NUM_MUTEXES] - 1) {
           return false;
       }
   }

   return true;
}

void dataTable::resizeInstructionStringVector(int x, int y) {
   pd_obj.instruction_strings.resize(y,std::vector<std::string>(x,""));
}

void dataTable::checkUpdateGridSize() {
   int num_thread_rows = pd_obj.parameter_input_data[(int)IPT_NUM_THREADS];
   int num_thread_cols = pd_obj.parameter_input_data[(int)IPT_INSTPAIRS_PERTHREAD] * 2;
   if (rows() != num_thread_rows || cols() != num_thread_cols) {
       rows(num_thread_rows);
       cols(num_thread_cols);
       std::vector<std::vector<std::string>>().swap(pd_obj.instruction_strings);
       resizeInstructionStringVector(cols(),rows());
   }
}

void dataTable::setInstructionString(int x, int y, std::string inst_str) {
   if (isValidInstructionString(inst_str) || inst_str == "") {
       pd_obj.instruction_strings[y][x] = inst_str;
   }
}

void dataTable::setInstructionStringThenHideInput() {
   std::string inp_str(current_inp->value());
   setInstructionString(in_column,in_row,inp_str);
   current_inp->hide();
   window()->cursor(FL_CURSOR_DEFAULT);
}

void dataTable::resetEdit() {
   if (current_inp->visible()) {
       setInstructionStringThenHideInput();
   }
}

void dataTable::beginEdit(TableContext tc, int r, int c) {
   edit_area = tc;
   in_row = r;
   in_column = c;
   int x,y,w,h;
   find_cell(tc,r,c,x,y,w,h);
   current_inp->resize(x,y,w,h);
   if (tc == CONTEXT_CELL) {
       current_inp->value(&(pd_obj.instruction_strings[r][c])[0]);
   }
   current_inp->show();
   current_inp->take_focus();
}

void currentDataTableInput_CB(Fl_Widget *w, void *data) {
   dataTable *dt = (dataTable *)data;
   dt->resetEdit();
}

// The callback function for when the input data table
// (bottom right portion) is clicked.
void dataTable_CB(Fl_Widget *w, void *data) {
   dataTable *dt = (dataTable *)w;
   // get the currently selected row
   int r = dt->callback_row();
   // get the currently selected column
   int c = dt->callback_col();
   // get the current table context (what is being focused on)
   Fl_Table::TableContext tc = dt->callback_context();
   switch(tc) {
      case(Fl_Table::CONTEXT_CELL): {
              switch (Fl::event()) {
                  case(FL_PUSH):
                       dt->resetEdit();
                       dt->beginEdit(tc,r,c);
                       break;
                  default:
                       break;
              }
              return;
      }
      case(Fl_Table::CONTEXT_TABLE): {
           dt->resetEdit();
           return;
      }
      default:
           break;
   }
}

