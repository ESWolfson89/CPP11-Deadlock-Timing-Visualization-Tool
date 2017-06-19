/*

Eric Wolfson
generatedstringdata.h
Contains static string data used by the code generator

*/

#ifndef GENERATEDSTRINGDATA_H_
#define GENERATEDSTRINGDATA_H_

#include "includes.h"

#define NUM_GENERATED_PREPROCESSOR_STRINGS 9
#define NUM_GENERATED_GLOBALDATA_STRINGS 11
#define NUM_GENERATED_TIMEOUTFUNC_STRINGS 3
#define NUM_GENERATED_UPDATEFUNC_STRINGS 21
#define NUM_GENERATED_SLEEPFUNC_STRINGS 5
#define NUM_GENERATED_LOCKFUNC_STRINGS 12
#define NUM_GENERATED_UNLOCKFUNC_STRINGS 8
#define NUM_GENERATED_UNLOCKALLFUNC_STRINGS 5
#define NUM_GENERATED_INITDATAFUNC_STRINGS 16
#define NUM_GENERATED_OUTPUTFUNC_STRINGS 14
#define NUM_GENERATED_MAINFUNCTOP_STRINGS 3
#define NUM_GENERATED_MAINFUNCBOTTOM_STRINGS 5

// includes needed in the generated code as well as some global defines
static const std::string generated_preprocessor_strings[NUM_GENERATED_PREPROCESSOR_STRINGS] = {
   "#include <iostream>\n",
   "#include <thread>\n",
   "#include <mutex>\n",
   "#include <array>\n",
   "#include <vector>\n",
   "#include <chrono>\n\n",
   "#include <fstream>\n\n",
   "using namespace std;\n\n",
   "#define FINAL_TIMEPOINT_VALUE (program_start_time + chrono::milliseconds(program_duration))\n\n"
};

// There are 3 events that can occur
// 1) Request lock
// 2) Acquire lock (assign)
// 3) relinquish lock (unlock)
// Every time one of these events occurs,
// the corresponding resource allocation graph data is stored in
// a vector to be processed later by the main front-end program

// locked_flags indicates the current possession of each thread with each
// mutex in the form of an adjacency matrix

// Note: "update_mutex" is needed to prevent a race condition occurring with the previously
// mentioned vector.
static const std::string generated_globaldata_strings[NUM_GENERATED_GLOBALDATA_STRINGS] = {
   "enum event_type {ET_REQUEST, ET_ASSIGN, ET_RELINQUISH};\n\n",
   "struct event_data {\n",
   "   unsigned long long int time_period;\n",
   "   int requests[NUM_THREADS];\n",
   "   int assignments[NUM_MUTEXES];\n",
   "};\n\n",
   "vector<event_data> all_data;\n\n",
   "bool locked_flags[NUM_MUTEXES][NUM_THREADS];\n\n",
   "array <timed_mutex,NUM_MUTEXES> mutexes;\n\n",
   "mutex update_mutex;\n\n",
   "chrono::steady_clock::time_point program_start_time;\n\n"
};

static const std::string generated_timeoutfunc_strings[NUM_GENERATED_TIMEOUTFUNC_STRINGS] = {
   "bool programTimeout() {\n",
   "   return chrono::steady_clock::now() >= FINAL_TIMEPOINT_VALUE;\n",
   "}\n\n"
};

// While the (generated) concurrent C++11 program is running, it collects data
// about threads and when they requested, required, and relinquished ownership of mutexes.
// The time at which each event occurs, as well as the current resource allocation graph
// data is stored in the "all_data" vector.
// The time period represents at what point the resource allocation graph
// was recorded in nanoseconds since the top of the generated c++11 program's main function.
static const std::string generated_updatefunc_strings[NUM_GENERATED_UPDATEFUNC_STRINGS] = {
   "void updateEventData(int mut, int th, event_type et) {\n",
   "   update_mutex.lock();\n",
   "   event_data ed = all_data[(int)all_data.size()-1];\n",
   "   ed.time_period = chrono::duration_cast<chrono::nanoseconds> (chrono::steady_clock::now() - program_start_time).count();\n",
   "   switch(et) {\n",
   "      case(ET_REQUEST):\n",
   "         ed.requests[th] = mut;\n",
   "         break;\n",
   "      case(ET_ASSIGN):\n",
   "         ed.requests[th] = -1;\n",
   "         ed.assignments[mut] = th;\n",
   "         break;\n",
   "      case(ET_RELINQUISH):\n",
   "         ed.assignments[mut] = -1;\n",
   "         break;\n",
   "      default:\n",
   "         break;\n",
   "   }\n",
   "   all_data.push_back(ed);\n",
   "   update_mutex.unlock();\n",
   "}\n\n"
};

// Called when a thread invokes a sleep instruction.
static const std::string generated_sleepfunc_strings[NUM_GENERATED_SLEEPFUNC_STRINGS] = {
   "void sleep(int ms) {\n",
   "   if (programTimeout())\n",
   "      return;\n",
   "   this_thread::sleep_for(chrono::milliseconds(ms));\n",
   "}\n\n"
};

// Called when a thread invokes a lock instruction.
// Data is only recorded if the lock is not acquired by this thread,
// and the program didn't timeout. The request time point is recorded
// as well as the acquire time point.
static const std::string generated_lockfunc_strings[NUM_GENERATED_LOCKFUNC_STRINGS] = {
   "void lock(int mut, int th) {\n",
   "   if (programTimeout())\n",
   "      return;\n",
   "   if (!locked_flags[mut][th]) {\n",
   "      updateEventData(mut,th,ET_REQUEST);\n",
   "      if(mutexes[mut].try_lock_until(FINAL_TIMEPOINT_VALUE)) {\n",
   "         if(!programTimeout())\n",
   "            updateEventData(mut,th,ET_ASSIGN);\n",
   "         locked_flags[mut][th] = true;\n",
   "      }\n",
   "   }\n",
   "}\n\n"
};

// Called when a thread executes an unlock instruction
// Data is only recorded if the lock is acquired by this thread, 
// and the program didn't timeout. The relinquish time point is recorded
static const std::string generated_unlockfunc_strings[NUM_GENERATED_UNLOCKFUNC_STRINGS] = {
   "void unlock(int mut, int th) {\n",
   "   if (locked_flags[mut][th]) {\n",
   "      mutexes[mut].unlock();\n",
   "      if(!programTimeout())\n",
   "         updateEventData(mut,th,ET_RELINQUISH);\n",
   "      locked_flags[mut][th] = false;\n",
   "   }\n",
   "}\n\n"
};

// Only called if a thread successfully finishes the last instruction,
// for clean-up purposes.
static const std::string generated_unlockallfunc_strings[NUM_GENERATED_UNLOCKALLFUNC_STRINGS] = {
   "void unlockAll(int th) {\n",
   "   for (int i = 0; i < NUM_MUTEXES; ++i) {\n",
   "      unlock(i,th);\n",
   "   }\n",
   "}\n\n"
};

// Initialize first resource allocation graph and other global data
// such as the current binary adjacency matrix representing which locks are
// held by which threads.
static const std::string generated_initdatafunc_strings[NUM_GENERATED_INITDATAFUNC_STRINGS] = {
   "void initData() {\n",
   "   for (int i = 0; i < NUM_MUTEXES; ++i) {\n",
   "      for (int j = 0; j < NUM_THREADS; ++j) {\n",
   "         locked_flags[i][j] = false;\n",
   "      }\n",
   "   }\n\n",
   "   event_data ed;\n\n",
   "   ed.time_period = 0ULL;\n\n",
   "   for (int i = 0; i < NUM_THREADS; ++i) {\n",
   "      ed.requests[i] = -1;\n",
   "   }\n\n",
   "   for (int i = 0; i < NUM_MUTEXES; ++i) {\n",
   "      ed.assignments[i] = -1;\n",
   "   }\n\n",
   "   all_data.push_back(ed);\n",
   "}\n\n"
};

// Store event data into temporary file that will be processed by the GUI program
static const std::string generated_outputfunc_strings[NUM_GENERATED_OUTPUTFUNC_STRINGS] = {
   "void outputResults() {\n",
   "   ofstream result_file(\"unprocessedResultsTemp.txt\");\n\n",
   "   for (int i = 0; i < (int)all_data.size(); ++i) {\n",
   "      result_file << all_data[i].time_period << \" \";\n",
   "      for (int j = 0; j < NUM_THREADS; ++j) {\n",
   "         result_file << \"T\" << j << \"-\" << (all_data[i].requests[j]!=-1?\"M\"+to_string(all_data[i].requests[j]):\"N\") << \" \";\n",
   "      }\n",
   "      for (int j = 0; j < NUM_MUTEXES; ++j) {\n",
   "         result_file << \"M\" << j << \"-\" << (all_data[i].assignments[j]!=-1?\"T\"+to_string(all_data[i].assignments[j]):\"N\") << \" \";\n",
   "      }\n",
   "      result_file << \"\\n\";\n",
   "   }\n\n",
   "   result_file.close();\n",
   "}\n\n"
};

// The next two sets of strings comprise the main function
// of the generated C++11 program
// 1) Initialize global data
// 2) Get program start time
static const std::string generated_mainfunctop_strings[NUM_GENERATED_MAINFUNCTOP_STRINGS] = {
   "int main() {\n",
   "   initData();\n\n",
   "   program_start_time = chrono::steady_clock::now();\n\n"
};

// In between these two sets of strings is where the threads are run and joined

// 3) Output unprocessed results to file and then exit
static const std::string generated_mainfuncbottom_strings[NUM_GENERATED_MAINFUNCBOTTOM_STRINGS] = {
   "\n",
   "   outputResults();\n\n",
   "   all_data.clear();\n\n",
   "   return 0;\n",
   "}\n"
};

#endif
