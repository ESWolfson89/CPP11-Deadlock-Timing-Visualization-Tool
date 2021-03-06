/*

Eric Wolfson
resultsanalyzer.h
Analyzes resource allocation graph files logged by generated C++11 files
Computes graph cycles

*/

#ifndef RESULTSANALYZER_H_
#define RESULTSANALYZER_H_

#include "graph.h"

// This class analyzes the unprocessed results created
// from the concurrent c++11 programs. It finds all cycles in the 
// resource allocation graphs contained in the .unpr files and stores
// them in .ragc files
class resultsAnalyzer {
   public:
      resultsAnalyzer();
      resultsAnalyzer(int,int);
      void setNumMutexesAndThreads(int,int);
      void setInputFilePrefixString(std::string);
      void setRunCountIndicatorString(std::string);
      // Reads in the file generated by the concurrent C++11 program
      void preprocessInputStrings();
      // ... and then parse RAG strings and store into "graph" format
      void generateResourceAllocationGraphs();
      // Runs Tarjan for all RAGS. Stores cycle data in .ragc files.
      void computeCyclesForAllGraphs();
      // Algorithm for finding all the cycles in the resource allocation graph is called:
      // "Tarjan's Algorithm"
      void tarjanAlgorithm(graph *, std::vector<std::string> &);
      // Sub-function needed in Tarjan's algorithm (the above function is technically a helper)
      // This is a recursive function.
      void strongConnect(graph *, int &, int, std::vector<std::string> &, std::stack<node> &);
      void cleanUp();
      int getRagGraphVectorSize();
      int getRagTimestampVectorSize();
      int getRagCyclesVectorSize();
      int getRagCyclesVectorSizeForTimestamp(int);
      uint64 getRagTimestamp(int);
      graph *getRagGraph(int);
      std::string getRagCycle(int,int);
   private:
      std::vector< std::vector<std::string> > rag_cycles;
      std::vector<std::string> input_strings;
      std::vector<graph> rag_objs;
      std::vector<uint64> rag_timestamps;
      std::string inp_file_prefix_str;
      std::string run_count_indicator_str;
      int num_mutexes;
      int num_threads;
};

// Analysis object
extern resultsAnalyzer analyzer_obj;

#endif
