/*

Eric Wolfson
executioncontrolbox.cpp
Controls various inputs into the program GUI

*/

#include "executioncontrolbox.h"

executionControlBox::executionControlBox(int x, int y, int w, int h) : Fl_Box(x,y,w,h,0) {
    color(FL_BACKGROUND_COLOR);
    box(FL_PLASTIC_UP_BOX);
    controlbox_label = new Fl_Box(x+(w/2),48,1,1,"PARAMETER CONTROLS");
    controlbox_label->align(FL_ALIGN_CENTER);
    initExecutionProgressIndicator();
    initPrimaryExecutionButton();
    initCreateInputRadioButton();
    initRandomInputRadioButton();
    initNumExecutionRunsIntegerInput();
};

void executionControlBox::initExecutionProgressIndicator() {
    execution_progress_indicator = new executionProgressBar(x(),y()+h()-32,w(),32);
    execution_progress_indicator->label("Ready");
}

void executionControlBox::initPrimaryExecutionButton() {
    primary_execution_button = new Fl_Button(x(),y()+h()-128,w(),96,"Execute And Peform Analysis");
    primary_execution_button->color(FL_GREEN);
    primary_execution_button->when(FL_WHEN_CHANGED);
    primary_execution_button->callback(primaryExecutionButton_CB,execution_progress_indicator);
}

void executionControlBox::initCreateInputRadioButton() {
    create_input_radiobutton = new Fl_Radio_Round_Button(x()+106,y()+36,64,24,"Generate File");
    create_input_radiobutton->type(FL_TOGGLE_BUTTON);
    create_input_radiobutton->align(FL_ALIGN_LEFT);
    create_input_radiobutton->when(FL_WHEN_CHANGED);
    create_input_radiobutton->set();
    create_input_radiobutton->callback(createInputRadioButton_CB);
}

void executionControlBox::initRandomInputRadioButton() {
    random_input_radiobutton = new Fl_Radio_Round_Button(x()+262,y()+36,64,24,"Create Random");
    random_input_radiobutton->type(FL_TOGGLE_BUTTON);
    random_input_radiobutton->align(FL_ALIGN_LEFT);
    random_input_radiobutton->when(FL_WHEN_CHANGED);
    random_input_radiobutton->set();
    random_input_radiobutton->callback(randomInputRadioButton_CB);
}

void executionControlBox::initNumExecutionRunsIntegerInput() {
    num_execution_runs_iinp = new Fl_Int_Input(x()+304,y()+36,64,24,"# runs");
    num_execution_runs_iinp->value("1");
    num_execution_runs_iinp->align(FL_ALIGN_TOP);
    num_execution_runs_iinp->callback(numExecutionRunsIInp_CB);
}

// When the user presses the large green button to execute the pipeline,
// this callback function is called.
void primaryExecutionButton_CB(Fl_Widget *w, void *data) {
   pd_obj.pipeline_running = true;
   w->deactivate();
   Fl::check();
   int num_execution_runs = pd_obj.num_execution_runs;
   int num_input_files = pd_obj.num_input_files;
   std::string inp_file_choice_str = pd_obj.inp_file_choice_str;
   bool create_input_flag = (pd_obj.ci_flag || inp_file_choice_str == "" || num_input_files == 0);
   if (((Fl_Button *)w)->value() == 1)
   {
      executionProgressBar *execution_progress_indicator = (executionProgressBar *)data;

      if (!create_input_flag) {
          for (int i = 0; i < 5; ++i) {
               inp_file_choice_str.pop_back();
          }
          gen_obj.setInputFilePrefixString(inp_file_choice_str);
      }
      else
          gen_obj.setInputFilePrefixString("pattern" + std::to_string(num_input_files+1));

      std::string rcis = "";

      std::vector<std::vector<graph>>().swap(pd_obj.displayed_graphs);
      std::vector<std::vector<int>>().swap(pd_obj.bar_positions);
      std::vector<processDeadlock>().swap(dlprocess_objs);

      // Each iteration of this loop is a run of the main pipeline
      for (int i = 0; i < num_execution_runs; ++i) {
         rcis = std::to_string(i+1);
         // Generate code (only once if multiple runs are produced. In this case, the same
         // executable is run several times, rather than recompiling the same file into the same executable
         // several times as well; this would be extra unneeded work)
         if (i == 0) {
             if (create_input_flag) {
                 reDisplayLabel(execution_progress_indicator,"Generating Input File " + rcis,(9*i),num_execution_runs);
                 gen_obj.genInputFile(&pd_obj);
             }
             reDisplayLabel(execution_progress_indicator,"Generating CPP File " + rcis,(9*i)+1,num_execution_runs);
             gen_obj.runCppGenerator();
             reDisplayLabel(execution_progress_indicator,"Compiling CPP File " + rcis,(9*i)+2,num_execution_runs);
             gen_obj.compileCpp();
         }

         reDisplayLabel(execution_progress_indicator,"Running CPP File " + rcis,(9*i)+3,num_execution_runs);
         gen_obj.runCpp();
         // Process data
         reDisplayLabel(execution_progress_indicator,"Preprocessing graphs " + rcis,(9*i)+4,num_execution_runs);
         analyzer_obj.setInputFilePrefixString(gen_obj.getInputFilePrefixString());
         analyzer_obj.setRunCountIndicatorString(std::to_string(i+1));
         analyzer_obj.setNumMutexesAndThreads(gen_obj.getNumMutexes(), gen_obj.getNumThreads());
         analyzer_obj.preprocessInputStrings();
         reDisplayLabel(execution_progress_indicator,"Storing graph data " + rcis,(9*i)+5,num_execution_runs);
         analyzer_obj.generateResourceAllocationGraphs();
         reDisplayLabel(execution_progress_indicator,"Computing graph cycles " + rcis,(9*i)+6,num_execution_runs);
         analyzer_obj.computeCyclesForAllGraphs();
         // store display data
         reDisplayLabel(execution_progress_indicator,"Storing display dataset " + rcis,(9*i)+7,num_execution_runs);
         populateDisplayGraphsVector();
         populateThreadBarPositionsVector();
         reDisplayLabel(execution_progress_indicator,"Pinpointing Deadlocks for run " + rcis,(9*i)+8,num_execution_runs);
         dlprocess_objs.push_back(processDeadlock());
         dlprocess_objs[i].populateDeadlockVector(&analyzer_obj);
         analyzer_obj.cleanUp();
      }
      reDisplayLabel(execution_progress_indicator,"Ready",0,num_execution_runs);
      ((Fl_Button *)w)->value(0);
   }
   w->activate();
   pd_obj.pipeline_running = false;
}

void randomInputRadioButton_CB(Fl_Widget *w, void *data) {
   Fl_Radio_Round_Button *rrb = (Fl_Radio_Round_Button *)w;
   if (rrb->value() == 0) {
       pd_obj.ri_flag = false;
   }
   else {
       pd_obj.ri_flag = true;
   }
}

void createInputRadioButton_CB(Fl_Widget *w, void *data) {
   Fl_Radio_Round_Button *rrb = (Fl_Radio_Round_Button *)w;
   if (rrb->value() == 0) {
       pd_obj.ci_flag = false;
   }
   else {
       pd_obj.ci_flag = true;
   }
}

void numExecutionRunsIInp_CB(Fl_Widget *w, void *data) {
   Fl_Int_Input *iinp = (Fl_Int_Input *)w;
   int new_val = std::atoi(iinp->value());
   if (new_val > 25 || new_val < 1) {
       new_val = 1;
       iinp->value("1");
   }
   pd_obj.num_execution_runs = new_val;
}


// helper for progress bar
void reDisplayLabel(executionProgressBar *epb, std::string s, int progress, int max_progress) {
   double progress_ratio = (double)progress/(9.0*(double)max_progress);
   epb->setProgressRatio(progress_ratio);
   epb->copy_label(&s[0]);
   epb->redraw();
   Fl::check();
}

// pd_obj.displayed_graphs contains temp data from the resultsAnalyzer class
void populateDisplayGraphsVector() {
   pd_obj.displayed_graphs.push_back(std::vector<graph>());
   int latest_graph_index = (int)pd_obj.displayed_graphs.size() - 1;
   for (int i = 0; i < analyzer_obj.getRagGraphVectorSize(); ++i) {
        pd_obj.displayed_graphs[latest_graph_index].push_back(*(analyzer_obj.getRagGraph(i)));
   }
}

// pd_obj.bar_positions contains temp data from the resultsAnalyzer class.
// This temp data scales down the time stamps (which can be billions of nanoseconds)
// to just [0 to 2000] pixels.
void populateThreadBarPositionsVector() {
   int num_timestamps = analyzer_obj.getRagTimestampVectorSize();
   pd_obj.max_pos_val = analyzer_obj.getRagTimestamp(num_timestamps-1);
   uint64 current_val = 0;
   double scaled_val = 0.0;
   pd_obj.bar_positions.push_back(std::vector<int>());
   int latest_bp_index = (int)pd_obj.bar_positions.size() - 1;
   for (int i = 0; i < num_timestamps; ++i) {
        current_val = analyzer_obj.getRagTimestamp(i);
        // If max_pos_val is 0.0, the thread bars won't be displayed anyway,
        // but this just prevents an arithmetic exception from occurring, or
        // storing "nan" values...
        if (pd_obj.max_pos_val <= 0.0) {
            scaled_val = 1.0;
        }
        else {
            scaled_val = (double)(max_analysis_chart_bar_width*current_val)/(double)pd_obj.max_pos_val;
        }
        pd_obj.bar_positions[latest_bp_index].push_back((int)scaled_val);
   }
}


