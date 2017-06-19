/*

Eric Wolfson
inputcontrolgroup.cpp
Class for input table object and relevant widgets

*/

#include "inputcontrolgroup.h"

inputControlGroup::inputControlGroup() {
}

inputControlGroup::inputControlGroup(int dx, int dy, int dw, int dh, int dr, int dc) {
   table_obj = new dataTable(dx,dy,dw,dh,dr,dc);
   tabel_label = new Fl_Box(dx*2,dy-8,1,1,"INPUT INSTRUCTIONS");
   tabel_label->align(FL_ALIGN_CENTER);
   initCurrentInputParametersIntegerInputs();
   initParameterInputFilesBrowser();
   initFileRefreshButton();
}

dataTable *inputControlGroup::getDataTable() {
   return table_obj;
}

Fl_Int_Input *inputControlGroup::getCurrentInputParameterIInp(int i) {
   return current_input_parameters_iinp[i];
}

void inputControlGroup::initParameterInputFilesBrowser() {
    input_parameters_fb = new Fl_File_Browser(230,240,148,296,"Parameter Files");
    setupFileBrowser(input_parameters_fb);
    input_parameters_fb->callback(selectInputFile_CB,(void *)this);
}

void inputControlGroup::initFileRefreshButton() {
    input_file_refresh_button = new Fl_Button(230,540,148,32,"Refresh");
    input_file_refresh_button->color(FL_BLACK);
    input_file_refresh_button->labelcolor(FL_WHITE);
    input_file_refresh_button->when(FL_WHEN_CHANGED);
    input_file_refresh_button->callback(inputFileRefreshButton_CB,input_parameters_fb);
}

// Set the seven random input parameter widget attributes.
void inputControlGroup::initCurrentInputParametersIntegerInputs() {
    for (int i = 0; i < NUM_INPUT_PARAMETER_TYPES; ++i) {
         current_input_parameters_iinp[i] = new Fl_Int_Input(160,224 + (i*48),64,48,&(current_input_parameters_name_str[i])[0]);
         current_input_parameters_iinp[i]->align(FL_ALIGN_LEFT);
         current_input_parameters_iinp[i]->value(&(std::to_string(pd_obj.parameter_input_data[i]))[0]);
         current_input_parameters_iinp[i]->when(FL_WHEN_ENTER_KEY_ALWAYS|current_input_parameters_iinp[i]->when());
         switch((input_parameter_type)i) {
            case(IPT_PROGRAM_DURATION):
               current_input_parameters_iinp[i]->callback(programDurationIInp_CB);
               break;
            case(IPT_NUM_MUTEXES):
               current_input_parameters_iinp[i]->callback(numMutexesIInp_CB);
               break;
            case(IPT_NUM_THREADS):
               current_input_parameters_iinp[i]->callback(numThreadsIInp_CB,table_obj);
               break;
            case(IPT_MIN_SLEEPVAL):
               current_input_parameters_iinp[i]->callback(minSleepValIInp_CB);
               break;
            case(IPT_MAX_SLEEPVAL):
               current_input_parameters_iinp[i]->callback(maxSleepValIInp_CB);
               break;
            case(IPT_INSTPAIRS_PERTHREAD):
               current_input_parameters_iinp[i]->callback(numThreadInstructionPairsIInp_CB,table_obj);
               break;
            case(IPT_LOCK_INSTCHANCE):
               current_input_parameters_iinp[i]->callback(lockChanceIInp_CB);
               break;
            default:
               break;
         }
    }
}

// callback implementations
void programDurationIInp_CB(Fl_Widget *w, void *data) {
   Fl_Int_Input *iinp = (Fl_Int_Input *)w;
   int new_val = std::atoi(iinp->value());
   int min_possible_val = (pd_obj.parameter_input_data[(int)IPT_MAX_SLEEPVAL] + 1) *
                          pd_obj.parameter_input_data[(int)IPT_INSTPAIRS_PERTHREAD];
   if (new_val > max_input_program_duration ||
       new_val < min_input_program_duration) {
       new_val = 1000;
       iinp->value("1000");
   }
   if (new_val < min_possible_val) {
       new_val = min_possible_val;
       iinp->value(&(std::to_string(min_possible_val))[0]);
   }
   pd_obj.parameter_input_data[(int)IPT_PROGRAM_DURATION] = new_val;
}

void numMutexesIInp_CB(Fl_Widget *w, void *data) {
   Fl_Int_Input *iinp = (Fl_Int_Input *)w;
   int new_val = std::atoi(iinp->value());
   if (new_val > max_input_num_mutexes ||
       new_val < min_input_num_mutexes) {
       new_val = 2;
       iinp->value("2");
   }
   pd_obj.parameter_input_data[(int)IPT_NUM_MUTEXES] = new_val;
}

void numThreadsIInp_CB(Fl_Widget *w, void *data) {
   Fl_Int_Input *iinp = (Fl_Int_Input *)w;
   dataTable *dt = (dataTable *)data;
   int new_val = std::atoi(iinp->value());
   if (new_val > max_input_num_threads ||
       new_val < min_input_num_threads) {
       new_val = 2;
       iinp->value("2");
   }
   pd_obj.parameter_input_data[(int)IPT_NUM_THREADS] = new_val;
   dt->checkUpdateGridSize();
}

void minSleepValIInp_CB(Fl_Widget *w, void *data) {
   Fl_Int_Input *iinp = (Fl_Int_Input *)w;
   int new_val = std::atoi(iinp->value());
   if (new_val > max_input_min_sleepval ||
       new_val < min_input_min_sleepval ||
       new_val > pd_obj.parameter_input_data[(int)IPT_MAX_SLEEPVAL]) {
       new_val = pd_obj.parameter_input_data[(int)IPT_MAX_SLEEPVAL];
       iinp->value(&(std::to_string(new_val))[0]);
   }
   pd_obj.parameter_input_data[(int)IPT_MIN_SLEEPVAL] = new_val;
}

void maxSleepValIInp_CB(Fl_Widget *w, void *data) {
   Fl_Int_Input *iinp = (Fl_Int_Input *)w;
   int new_val = std::atoi(iinp->value());
   int max_possible_val = pd_obj.parameter_input_data[(int)IPT_PROGRAM_DURATION] /
                          (pd_obj.parameter_input_data[(int)IPT_INSTPAIRS_PERTHREAD] + 1);
   if (new_val > max_input_max_sleepval ||
       new_val < min_input_max_sleepval ||
       new_val < pd_obj.parameter_input_data[(int)IPT_MIN_SLEEPVAL]) {
       new_val = pd_obj.parameter_input_data[(int)IPT_MIN_SLEEPVAL];
       iinp->value(&(std::to_string(new_val))[0]);
   }
   if (new_val > max_possible_val) {
       new_val = max_possible_val;
       iinp->value(&(std::to_string(max_possible_val))[0]);
   }
   pd_obj.parameter_input_data[(int)IPT_MAX_SLEEPVAL] = new_val;
}

void numThreadInstructionPairsIInp_CB(Fl_Widget *w, void *data) {
   Fl_Int_Input *iinp = (Fl_Int_Input *)w;
   dataTable *dt = (dataTable *)data;
   int new_val = std::atoi(iinp->value());
   int max_possible_val = pd_obj.parameter_input_data[(int)IPT_PROGRAM_DURATION] /
                          (pd_obj.parameter_input_data[(int)IPT_MAX_SLEEPVAL] + 1);
   if (new_val > max_input_instrpairs_perthread ||
       new_val < min_input_instrpairs_perthread) {
       new_val = 4;
       iinp->value("4");
   }
   if (new_val > max_possible_val) {
       new_val = max_possible_val;
       iinp->value(&(std::to_string(max_possible_val))[0]);
   }
   pd_obj.parameter_input_data[(int)IPT_INSTPAIRS_PERTHREAD] = new_val;
   dt->checkUpdateGridSize();
}

void lockChanceIInp_CB(Fl_Widget *w, void *data) {
   Fl_Int_Input *iinp = (Fl_Int_Input *)w;
   int new_val = std::atoi(iinp->value());
   if (new_val > max_input_lock_instrchance ||
       new_val < min_input_lock_instrchance) {
       new_val = 50;
       iinp->value("50");
   }
   pd_obj.parameter_input_data[(int)IPT_LOCK_INSTCHANCE] = new_val;
}

void inputFileRefreshButton_CB(Fl_Widget *w, void *data) {
   if (((Fl_Button *)w)->value() == 1) {
       Fl_File_Browser *fb = (Fl_File_Browser *)data;
       setupFileBrowser(fb);
   }
}

void selectInputFile_CB(Fl_Widget *w, void *data) {
   Fl_File_Browser *fb = (Fl_File_Browser *)w;
   inputControlGroup *icg = (inputControlGroup *)data;
   int line_no = fb->value();
   if (line_no >= 1 && line_no <= fb->size()) {
      std::string new_str(fb->text(line_no));
      pd_obj.inp_file_choice_str = new_str;
   }
   if (line_no <= 0 && fb->size() >= 1) {
      std::string new_str(fb->text(1));
      pd_obj.inp_file_choice_str = new_str;
   }
   loadInputFileToGrid(icg);
}

// helpers

// This function is called at GUI initialization and when
// the "Refresh" button below the file browser is pressed
void setupFileBrowser(Fl_File_Browser *fb) {
    fb->filter("*.inpf");
    fb->load(".");
    fb->type(FL_HOLD_BROWSER);
    fb->align(FL_ALIGN_TOP);
    fb->remove(1);
    fb->when(FL_WHEN_CHANGED);
    fb->callback(selectInputFile_CB);
    pd_obj.num_input_files = fb->size();
}

// Loads into dataTable widget the selected input file from the file browser widget.
// Populates each cell with the corresponding instructions.
void loadInputFileToGrid(inputControlGroup *icg) {
   std::string loaded_file_str = pd_obj.inp_file_choice_str;
   std::string num_threads_str = "1";
   std::string num_mutexes_str = "1";
   std::string program_duration_str = "1";
   std::string temp_added_str = "";
   std::vector<std::string> loaded_sets_of_instructions;
   int max_num_instructions = 1;
   int current_count = 1;
   int grid_x_iter = 0;
   int grid_pairs = 2;

   if (loaded_file_str == "") {
       return;
   }

   std::ifstream ifs;
   ifs.open(loaded_file_str);
   std::getline(ifs, program_duration_str);
   std::getline(ifs, num_mutexes_str);
   std::getline(ifs, num_threads_str);

   pd_obj.parameter_input_data[(int)IPT_NUM_THREADS] = std::stoi(num_threads_str.substr(2));
   pd_obj.parameter_input_data[(int)IPT_NUM_MUTEXES] = std::stoi(num_mutexes_str.substr(2));
   pd_obj.parameter_input_data[(int)IPT_PROGRAM_DURATION] = std::stoi(program_duration_str.substr(2));

   loaded_sets_of_instructions.resize(pd_obj.parameter_input_data[(int)IPT_NUM_THREADS]);

   for (int i = 0; i < pd_obj.parameter_input_data[(int)IPT_NUM_THREADS]; ++i) {
        std::getline(ifs,loaded_sets_of_instructions[i]);
        current_count = std::count(loaded_sets_of_instructions[i].begin(),loaded_sets_of_instructions[i].end(), ' ') + 1;
        if (max_num_instructions < current_count) {
            max_num_instructions = current_count;
        }
   }

   grid_pairs = (max_num_instructions + 1)/2;
   pd_obj.parameter_input_data[(int)IPT_INSTPAIRS_PERTHREAD] = grid_pairs;
   icg->getDataTable()->checkUpdateGridSize();

   for (int y = 0; y < (int)pd_obj.instruction_strings.size(); ++y) {
        grid_x_iter = 0;
        for (int c = 0; c < (int)loaded_sets_of_instructions[y].size(); ++c) {
             if (loaded_sets_of_instructions[y][c] != ' ') {
                 temp_added_str += loaded_sets_of_instructions[y][c];
             }
             else {
                 pd_obj.instruction_strings[y][grid_x_iter] = temp_added_str;
                 grid_x_iter++;
                 temp_added_str = "";
             }
        }
        pd_obj.instruction_strings[y][grid_x_iter] = temp_added_str;
        temp_added_str = "";
   }

   icg->getDataTable()->redraw();
   std::vector<std::string>().swap(loaded_sets_of_instructions);

   // Update random input parameter fields to reflect that of the newly loaded input file.
   icg->getCurrentInputParameterIInp((int)IPT_PROGRAM_DURATION)->value(&(std::to_string(pd_obj.parameter_input_data[(int)IPT_PROGRAM_DURATION]))[0]);
   icg->getCurrentInputParameterIInp((int)IPT_NUM_THREADS)->value(&(std::to_string(pd_obj.parameter_input_data[(int)IPT_NUM_THREADS]))[0]);
   icg->getCurrentInputParameterIInp((int)IPT_NUM_MUTEXES)->value(&(std::to_string(pd_obj.parameter_input_data[(int)IPT_NUM_MUTEXES]))[0]);
   icg->getCurrentInputParameterIInp((int)IPT_INSTPAIRS_PERTHREAD)->value(&(std::to_string(pd_obj.parameter_input_data[(int)IPT_INSTPAIRS_PERTHREAD]))[0]);
}

