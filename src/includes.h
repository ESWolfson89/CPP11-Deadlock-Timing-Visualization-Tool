/*

Eric Wolfson
Includes.h
Contains STL and standard libraries
Also includes some reused defines

*/

#ifndef INCLUDES_H_
#define INCLUDES_H_

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <fstream>
#include <random>
#include <algorithm>
#include <regex>
#include <cmath>
#include <time.h>

#define uint64 unsigned long long int

#define NUM_INPUT_PARAMETER_TYPES 7

static const int max_input_program_duration = 600000;
static const int max_input_num_mutexes = 500;
static const int max_input_num_threads = 500;
static const int max_input_min_sleepval = 1000;
static const int max_input_max_sleepval = 1000;
static const int max_input_instrpairs_perthread = 500;
static const int max_input_lock_instrchance = 99;
static const int min_input_program_duration = 3;
static const int min_input_num_mutexes = 1;
static const int min_input_num_threads = 1;
static const int min_input_min_sleepval = 0;
static const int min_input_max_sleepval = 0;
static const int min_input_instrpairs_perthread = 1;
static const int min_input_lock_instrchance = 1;

// various static front end values
static const int max_analysis_chart_bar_width = 3000;
static const int bar_space_factor = 2;

static const std::string current_input_parameters_name_str[NUM_INPUT_PARAMETER_TYPES] = {
   "program timeout (ms)",
   "num usable mutexes",
   "num running threads",
   "minimum sleep value",
   "maximum sleep value",
   "num inst. pairs/thread",
   "lock inst. chance"
};

// Index values of global prog data's
// parameter_input_data array
enum input_parameter_type {
   IPT_PROGRAM_DURATION,
   IPT_NUM_MUTEXES,
   IPT_NUM_THREADS,
   IPT_MIN_SLEEPVAL,
   IPT_MAX_SLEEPVAL,
   IPT_INSTPAIRS_PERTHREAD,
   IPT_LOCK_INSTCHANCE
};

#endif
