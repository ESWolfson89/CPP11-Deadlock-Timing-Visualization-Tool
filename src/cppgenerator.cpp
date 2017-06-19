/*

Eric Wolfson
cppgenerator.cpp
class implementation for cppGenerator class

*/

#include "cppgenerator.h"

cppGenerator::cppGenerator() {
  program_duration = 1000;
  num_mutexes = num_threads = 1;
  instruction = instruction_line = "";
  inp_file_prefix_str = "";
}

void cppGenerator::setInputFilePrefixString(std::string str) {
  inp_file_prefix_str = str;
}

// Compile the current CPP file (i.e. pattern44gencode.cpp
void cppGenerator::compileCpp() {
  std::string generated_cpp_str = inp_file_prefix_str + "gencode";
  std::string compile_line_str = "g++-5 -std=c++11 -pthread " + generated_cpp_str + ".cpp -o " + generated_cpp_str;
  std::system(&compile_line_str[0]);
}

// Runs the current CPP file (i.e. ./pattern44gencode
// NOTE: This will only work on linux, because of the "./", I would imagine for it to run
// on windows it would need to be "pattern44gencode.exe".
void cppGenerator::runCpp() {
  std::string execute_line_str = "./" + inp_file_prefix_str + "gencode";
  std::system(&execute_line_str[0]);
}

// Helper for generating a random or manual input file.
void cppGenerator::genInputFile(global_prog_data *pd_param) {
  bool gen_randomly = pd_param->ri_flag;

  std::ofstream pfs;
  std::string inp_file_str = inp_file_prefix_str + ".inpf";

  pfs.open(&inp_file_str[0]);

  pfs << "DN" << pd_param->parameter_input_data[(int)IPT_PROGRAM_DURATION] << "\n";

  pfs << "MN" << pd_param->parameter_input_data[(int)IPT_NUM_MUTEXES] << "\n";
  pfs << "TN" << pd_param->parameter_input_data[(int)IPT_NUM_THREADS] << "\n";

  if (gen_randomly == true) {
      genRandomInputFile(pd_param,pfs);
  }
  else {
      genManuallyConfiguredInputFile(pd_param,pfs);
  }

  pfs.close();
}

// Generate manually specified input file.
// Needs global_prog_data's instruction_strings 2D vector
// If one of the threads has no instructions, just throw "S1" in as a lone instruction (Sleep 1 ms) by default
// Read from 2D vector and store corresponding strings in input (.inpf) file
void cppGenerator::genManuallyConfiguredInputFile(global_prog_data *pd_param, std::ofstream &pfs) {
  int stop_space_index;
  int num_instruction_slots;
  std::string actual_instruction_str = "";
  for (int t = 0; t < (int)pd_param->instruction_strings.size(); ++t) {
     num_instruction_slots = (int)pd_param->instruction_strings[t].size();
     stop_space_index = num_instruction_slots - 1;
     for (int i = num_instruction_slots - 1; i >= 0; --i) {
        actual_instruction_str = pd_param->instruction_strings[t][i];
        if (actual_instruction_str == "") {
            stop_space_index--;
        }
        else {
            break;
        }
     }
     if (stop_space_index <= -1) {
        pfs << "S1";
     }
     else {
        for (int i = 0; i < num_instruction_slots; ++i) {
           actual_instruction_str = pd_param->instruction_strings[t][i];
           if (actual_instruction_str != "") {
              pfs << actual_instruction_str;
              if (i < stop_space_index) {
                 pfs << " ";
              }
           }
        }
     }
     pfs << "\n";
  }
}

// Generate random input file based on the 7 random parameters specified in "global_prog_data" struct
void cppGenerator::genRandomInputFile(global_prog_data *pd_param, std::ofstream &pfs) {
  int rpid_num_mutexes = pd_param->parameter_input_data[(int)IPT_NUM_MUTEXES];
  int rpid_num_threads = pd_param->parameter_input_data[(int)IPT_NUM_THREADS];
  int rpid_min_sleepval = pd_param->parameter_input_data[(int)IPT_MIN_SLEEPVAL];
  int rpid_max_sleepval = pd_param->parameter_input_data[(int)IPT_MAX_SLEEPVAL];
  int rpid_instpairs_perthread = pd_param->parameter_input_data[(int)IPT_INSTPAIRS_PERTHREAD];
  int rpid_lock_instchance = pd_param->parameter_input_data[(int)IPT_LOCK_INSTCHANCE];

  for (int t = 0; t < rpid_num_threads; ++t) {
     for (int i = 0; i < rpid_instpairs_perthread; ++i) {
          pfs << "S" << randInt(rpid_min_sleepval,rpid_max_sleepval) << " ";
          pfs << (rollPercentage(rpid_lock_instchance) == true ? "L" : "U");
          pfs << randInt(0,rpid_num_mutexes - 1);
          if (i < rpid_instpairs_perthread - 1) {
              pfs << " ";
          }
     }
     pfs << "\n";
  }
}

// Generate the .cpp file.
// Most of the code generated is stored in the static strings above.
void cppGenerator::storeCppFileContents() {
  std::ofstream ofs;
  std::string generated_cpp_str = inp_file_prefix_str + "gencode.cpp";
  ofs.open(&generated_cpp_str[0]);

  for (int i = 0; i < NUM_GENERATED_PREPROCESSOR_STRINGS; ++i) {
       ofs << generated_preprocessor_strings[i];
  }

  ofs << "#define NUM_MUTEXES " << num_mutexes << "\n";
  ofs << "#define NUM_THREADS " << num_threads << "\n\n";

  ofs << "static const int program_duration = " << program_duration << ";\n\n";

  for (int i = 0; i < NUM_GENERATED_GLOBALDATA_STRINGS; ++i) {
       ofs << generated_globaldata_strings[i];
  }

  for (int i = 0; i < NUM_GENERATED_TIMEOUTFUNC_STRINGS; ++i) {
       ofs << generated_timeoutfunc_strings[i];
  }

  for (int i = 0; i < NUM_GENERATED_UPDATEFUNC_STRINGS; ++i) {
       ofs << generated_updatefunc_strings[i];
  }

  for (int i = 0; i < NUM_GENERATED_SLEEPFUNC_STRINGS; ++i) {
       ofs << generated_sleepfunc_strings[i];
  }

  for (int i = 0; i < NUM_GENERATED_LOCKFUNC_STRINGS; ++i) {
       ofs << generated_lockfunc_strings[i];
  }

  for (int i = 0; i < NUM_GENERATED_UNLOCKFUNC_STRINGS; ++i) {
       ofs << generated_unlockfunc_strings[i];
  }

  for (int i = 0; i < NUM_GENERATED_UNLOCKALLFUNC_STRINGS; ++i) {
       ofs << generated_unlockallfunc_strings[i];
  }

  // Here is where the generated programs mostly vary.
  // If an instruction string set for a given thread (say, thread 3) is:
  // "S100 L1 S10 U1 S100 L2 S100 L0 S50", the following function
  // (out of a set of "tfunc#" functions) is generated:
  // void tfunc3() {
  //      sleep(100);
  //      lock(1,3);
  //      sleep(10);
  //      unlock(1,3);
  //      sleep(100);
  //      lock(2,3);
  //      sleep(100);
  //      lock(0,3);
  //      sleep(50);
  //      unlockAll(3);
  // }
  // When a thread finishes its final instruction "sleep(50)", it unlocks
  // all its mutexes to prevent undefined behavior of a thread ending with its mutexes held.
  for (int i = 0; i < num_threads; ++i) {
       ofs << "void tfunc" << (i+1) << "() {\n";
       for (int j = 0; j < (int)actions[i].size(); ++j) {
            switch (actions[i][j][0]) {
                    case('S'):
                         ofs << "   sleep(" << actions[i][j].substr(1) << ");\n";
                         break;
                    case('L'):
                         ofs << "   lock(" << actions[i][j].substr(1) << "," << i << ");\n";
                         break;
                    case('U'):
                         ofs << "   unlock(" << actions[i][j].substr(1) << "," << i << ");\n";
                         break;
                    default:
                         break;
            }
       }
       ofs << "   unlockAll(" << i << ");\n}\n\n";
  }

  for (int i = 0; i < NUM_GENERATED_INITDATAFUNC_STRINGS; ++i) {
       ofs << generated_initdatafunc_strings[i];
  }

  for (int i = 0; i < NUM_GENERATED_OUTPUTFUNC_STRINGS; ++i) {
       ofs << generated_outputfunc_strings[i];
  }

  for (int i = 0; i < NUM_GENERATED_MAINFUNCTOP_STRINGS; ++i) {
       ofs << generated_mainfunctop_strings[i];
  }

  // Launch all threads.
  // Example code for 3 threads:
  // thread th1(&tfunc1);
  // thread th2(&tfunc2);
  // thread th3(&tfunc3);
  for (int i = 0; i < num_threads; ++i) {
       ofs << "   thread th" << (i+1) << "(&tfunc" << (i+1) << ");\n";
  }

  ofs << "\n";

  // Join all threads.
  // Example code for 3 threads:
  // tfunc1.join();
  // tfunc2.join();
  // tfunc3.join();
  for (int i = 0; i < num_threads; ++i) {
       ofs << "   th" << (i+1) << ".join();\n";
  }

  for (int i = 0; i < NUM_GENERATED_MAINFUNCBOTTOM_STRINGS; ++i) {
       ofs << generated_mainfuncbottom_strings[i];
  }

  ofs.close();
}

// Parse the .inpf file
void cppGenerator::loadParameterFileContents() {
  std::ifstream ifs;
  std::string inp_file_str = inp_file_prefix_str + ".inpf";
  ifs.open(&inp_file_str[0]);
  loadQuantities(&ifs);
  loadInstructionStrings(&ifs);
  ifs.close();
}

// Parse the .inpf file and store results in .cpp file
void cppGenerator::runCppGenerator() {
  loadParameterFileContents();
  storeCppFileContents();
  cleanUp();
}

void cppGenerator::addStringToVectorResetString() {
  actions[(int)actions.size()-1].push_back(instruction);
  instruction = "";
}

// The first 3 lines of the .inpf file should be of the form
// DN<X>
// MN<Y>
// TN<Z>
// ... not including the angle brackets, where X is program duration,
// Y is the number of mutexes, and Z is the number of threads
void cppGenerator::loadQuantities(std::ifstream *ifs) {
  std::string num_mutexes_str, num_threads_str, program_duration_str;

  std::getline(*ifs, program_duration_str);
  std::getline(*ifs, num_mutexes_str);
  std::getline(*ifs, num_threads_str);

  program_duration = std::stoi(program_duration_str.substr(2));
  num_mutexes = std::stoi(num_mutexes_str.substr(2));
  num_threads = std::stoi(num_threads_str.substr(2));
}

// Represents the parsing of lines 4 onward of .inpf file
// The remaining lines of the .inpf file should contain the instructions for each thread
// The first instruction line (line 4) represents the instructions for the first thread,
// the second instruction line (line 5) represents the instructions for the second thread,
// etc... Each instruction is stored in a 2D vector of strings called "actions".
// If we are accessing "actions[i][j]", then we are fetching the "j"th
// instruction of the "i"th thread.
void cppGenerator::loadInstructionStrings(std::ifstream *ifs) {
  char temp_char;

  while ( std::getline(*ifs,instruction_line) ) {
     actions.push_back(std::vector<std::string>());
     for (int i = 0; i < (int)instruction_line.size(); ++i) {
          temp_char = instruction_line[i];
          if (temp_char != ' ') 
              instruction += temp_char;
          else
              addStringToVectorResetString();
     }
     addStringToVectorResetString();
  }
}

// Reset actions vector for the next .inpf file to be parsed.
void cppGenerator::cleanUp() {
  std::vector<std::vector<std::string>>().swap(actions);
}

int cppGenerator::getNumMutexes() {
  return num_mutexes;
}

int cppGenerator::getNumThreads() {
  return num_threads;
}

std::string cppGenerator::getInputFilePrefixString() {
  return inp_file_prefix_str;
}

// Generator object
cppGenerator gen_obj;
