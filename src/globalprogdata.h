/*

Eric Wolfson
globalprogdata.h
struct used for keeping track of various front end constructs

*/

#ifndef GLOBALPROGDATA_H_
#define GLOBALPROGDATA_H_

#include "graph.h"

// Data shared by all the GUI widgets;
// consists of both frontend and backend data...
struct global_prog_data {
   // String representing prefix of files for current pipeline run.
   // "pattern1", "pattern2", "pattern3", etc... (for instance)
   std::string inp_file_choice_str;
   // Num execution runs for given input file
   int num_execution_runs;
   // Number of input files ending in .inpf that program detects in subdirectory
   int num_input_files;
   // current value of analysis chart slider
   int ac_vector_index_value;
   // This flag determines whether a new input file is created
   // when the primary execution  button is pressed.
   bool ci_flag;
   // If the above flag is set, this flag determines whether or not
   // a random or manually created input file is created.
   bool ri_flag;
   // Index 0: Program duration
   // Index 1: Num mutexes
   // Index 2: Num threads
   // Index 3: Minimum possible sleep value for sleep instructions (in ms)
   //          if ci_flag and ri_flags are set.
   // Index 4: ... maximum possible sleep value
   // Index 5: number of instruction pairs (i.e. ({Sleep, lock},{Sleep, unlock})
   // Index 6: percentage chance of instruction being lock instead of unlock in second
   //          element of pair.
   // 3,4 and 6 are only used if ri_flag is true
   // 5 only determines the size of the dataTable widget for manual input parameters if ri_flag
   //   is false, otherwise if ri_flag is true, then it determines the number of instruction pairs
   //   randomly generated for each thread...
   int parameter_input_data[NUM_INPUT_PARAMETER_TYPES];
   // Primary execution button was pressed and is not active (still running pipeline)
   bool pipeline_running;
   // Last timestamp for final resource allocation graph recorded
   uint64 max_pos_val;
   // Copy of all resource allocation graphs during last execution run
   // through main system pipeline (pipeline being compile->run->analyze->find cyeles->etc...)
   std::vector< std::vector<graph> > displayed_graphs;
   // Used for bar chart display. Scaled down from 0 to max_pos_val -> 0 to scrollable window size
   std::vector< std::vector<int> > bar_positions;
   // Used for manual inputs if ri_flag is false and ci_flag is true for next execution run.
   // Set by adding values to input grid widget.
   std::vector< std::vector<std::string> > instruction_strings;
};

// Stores various data needed by all widgets
extern global_prog_data pd_obj;

#endif
