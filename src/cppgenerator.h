/*

Eric Wolfson
cppgenerator.h
C++11 code generator class definition

*/

#ifndef CPPGENERATOR_H_
#define CPPGENERATOR_H_

#include "globalprogdata.h"
#include "generatedstringdata.h"
#include "randgen.h"

// 1) Generates CPP files representing concurrent C++11 programs
// 2) Also compiles and runs those files and stores intermediate resource allocation graph
//    results in .unpr (unprocessed) files (via the execution of the concurrent programs)
class cppGenerator {
  public:
    cppGenerator();
    // Input parameter is of the form "pattern<num>", where num is a positive integer
    // (angle brackets not included)
    void setInputFilePrefixString(std::string);
    // Sets a field that keeps track of which execution run from 1 to global_prog_data.num_execution_runs
    // is running (as a number string).
    void setRunCountIndicatorString(std::string);
    // Compile the current CPP file (i.e. pattern44gencode.cpp
    void compileCpp();
    // Runs the current CPP file (i.e. ./pattern44gencode
    // NOTE: This will only work on linux, because of the "./", I would imagine for it to run
    // on windows it would need to be "pattern44gencode.exe".
    void runCpp();
    // Generate CPP file (i.e. pattern44gencode.cpp) based on input file (i.e. pattern44.inpf)
    void runCppGenerator();
    // Generates a random input file with 7 possible random parameters described in the "global_prog_data"
    // struct field comments above.
    void genRandomInputFile(global_prog_data *,std::ofstream &);
    // The grid widget in the GUI is needed
    void genManuallyConfiguredInputFile(global_prog_data *,std::ofstream &);
    // Helper function for generating either a random input file or a manually created input file (via the GUI).
    void genInputFile(global_prog_data *);
    void addStringToVectorResetString();
    // After parsing the input file, the corresponding C++11 program source is stored in a .cpp file
    void storeCppFileContents();
    // Parse the .inpf file.
    // 1) Get program duration (timeout val in ms)
    // 2) Get num mutexes
    // 3) Get num threads
    // 4) Store all instructions in a 2D vector of strings
    void loadParameterFileContents();
    void loadQuantities(std::ifstream *);
    void loadInstructionStrings(std::ifstream *);
    void cleanUp();
    int getNumMutexes();
    int getNumThreads();
    std::string getInputFilePrefixString();
  private:
    int num_mutexes;
    int num_threads;
    int program_duration;
    std::string inp_file_prefix_str;
    std::string run_count_indicator_str;
    std::string instruction;
    std::string instruction_line;
    std::vector< std::vector<std::string> > actions;
};

// generator object
extern cppGenerator gen_obj;

#endif
