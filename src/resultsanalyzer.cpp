/*

Eric Wolfson
resultsanalyzer.cpp
Implementation of resultsanalyzer class

*/

#include "resultsanalyzer.h"

resultsAnalyzer::resultsAnalyzer() {
   num_mutexes = num_threads = 1;
   inp_file_prefix_str = "";
   run_count_indicator_str = "1";
}

resultsAnalyzer::resultsAnalyzer(int nm, int nt) {
   setNumMutexesAndThreads(nm,nt);
}

graph *resultsAnalyzer::getRagGraph(int i) {
   return &(rag_objs[i]);
}

uint64 resultsAnalyzer::getRagTimestamp(int i) {
   return rag_timestamps[i];
}

int resultsAnalyzer::getRagGraphVectorSize() {
   return (int)rag_objs.size();
}

int resultsAnalyzer::getRagTimestampVectorSize() {
   return (int)rag_timestamps.size();
}

int resultsAnalyzer::getRagCyclesVectorSize() {
   return (int)rag_cycles.size();
}

int resultsAnalyzer::getRagCyclesVectorSizeForTimestamp(int i) {
   return (int)rag_cycles[i].size();
}

std::string resultsAnalyzer::getRagCycle(int i, int j) {
   return rag_cycles[i][j];
}

void resultsAnalyzer::setNumMutexesAndThreads(int nm, int nt) {
   num_mutexes = nm;
   num_threads = nt;
}

void resultsAnalyzer::setInputFilePrefixString(std::string str) {
   inp_file_prefix_str = str;
}

void resultsAnalyzer::setRunCountIndicatorString(std::string str) {
   run_count_indicator_str = str;
}

void resultsAnalyzer::preprocessInputStrings() {
   std::string inp_line = "";

   uint64 timestamp = 0;

   std::ofstream ofs;
   std::ifstream ifs,ifsc;

   // This file is created for debugging purposes.
   std::string raw_graph_file_str = inp_file_prefix_str + "UNPR" + run_count_indicator_str + ".unpr";
   // Latest .unpr file generated is the one that is parsed.
   std::string temp_unprc_file_str = "unprocessedResultsTemp.txt";

   // copy to vectors
   ifs.open(&temp_unprc_file_str[0]);

   while ( !ifs.eof() ) {
     ifs >> timestamp;
     std::getline(ifs,inp_line);

     if (ifs.eof()) {
         break;
     }

     rag_timestamps.push_back(timestamp);
     input_strings.push_back(inp_line);
   }

   ifs.close();

   // Copy contents of temp file to named file.
   // We don't actually process the named .unpr file, since
   // the temp file is processed and reprocessed on the fly
   // with results stored in multidimensional vectors to be used later,
   // instead of reparsing each .unpr file whenever its data is needed.
   // Basically, .unpr files are created for debugging purposes.
   ifsc.open(&temp_unprc_file_str[0]);
   ofs.open(&raw_graph_file_str[0]);

   ofs << ifsc.rdbuf();

   ifsc.close();
   ofs.close();

   // Eliminate leading and trailing spaces for accurate parsing
   // At the time of adding in regex, the executable ballooned 5x the amount of disk space
   for (int i = 0; i < (int)input_strings.size(); ++i) {
      input_strings[i] = std::regex_replace(input_strings[i], std::regex("^ +"), "");
      input_strings[i] = std::regex_replace(input_strings[i], std::regex(" +$"), "");
   }
}

void resultsAnalyzer::generateResourceAllocationGraphs() {
   for (int i = 0; i < (int)input_strings.size(); ++i) {
      rag_objs.push_back(graph(num_mutexes,num_threads));
      addAllEdges(&(rag_objs[i]),input_strings[i]);
   }
}

// Helper function for Tarjan's Algorithm.
// Has a strange way of handling lone nodes,
// I fixed this problem by discarding all "lone node" cycles computed.
// (apparently a lone node was counted as a cycle before this).
void resultsAnalyzer::tarjanAlgorithm(graph *g, std::vector<std::string> & cycles) {
   int index = 0;
   std::stack<node> node_stack;
   for (int i = 0; i < g->getNumTotalNodes(); ++i) {
      if (g->getNode(i)->scc_index == -1) {
          strongConnect(g,index,i,cycles,node_stack);
      }
   }
}

// Most of the work of Tarjan's Algorithm is done here.
void resultsAnalyzer::strongConnect(graph *g, int &index, int i, std::vector<std::string> & cycles, std::stack<node> & node_stack) {
   if (g->getNode(i)->val <= -1)
       return;

   g->setNodeSCCIndex(i,index);
   g->setNodeSCCLowlink(i,index);
   index++;
   node_stack.push(*(g->getNode(i)));
   g->setNodeSCCOnStack(i,true);

   int ci = g->val2Index((g->getNode(i)->n_type == NT_MUTEX ? NT_THREAD : NT_MUTEX),g->getNode(i)->val);
   if (g->getNode(ci)->scc_index == -1) {
       strongConnect(g,index,ci,cycles,node_stack);
       g->setNodeSCCLowlink(i,std::min(g->getNode(i)->scc_lowlink,g->getNode(ci)->scc_lowlink));
   }
   else if (g->getNode(ci)->scc_onstack) {
       g->setNodeSCCLowlink(i,std::min(g->getNode(i)->scc_lowlink,g->getNode(ci)->scc_index));
   }

   int iters = 0;
   if (g->getNode(i)->scc_index == g->getNode(i)->scc_lowlink) {
       std::string new_scc_string = "";
       node temp_w;
       do {
          iters++;
          temp_w = node_stack.top();
          node_stack.pop();
          g->setNodeSCCOnStack(temp_w.vec_index,false);
          new_scc_string += (temp_w.n_type == NT_MUTEX ? "M" : "T");
          new_scc_string += (temp_w.n_type == NT_MUTEX ? std::to_string(temp_w.vec_index) :
                                                         std::to_string(temp_w.vec_index - num_mutexes));
          new_scc_string += " ";
       } while (temp_w.vec_index != g->getNode(i)->vec_index);
       if (iters > 1) {
          // At this point, we have a cycle stored in the form of "new_scc_string".
          // Save this cycle in a 2D vector.
          // The inner most dimension represents a RAG, while the outermost dimension represents a cycle in RAG.
          // Each RAG can have more than one cycle, hence the 2D vector.
          cycles.push_back(new_scc_string);
       }
   }
}

void resultsAnalyzer::computeCyclesForAllGraphs() {
   std::vector<std::string> cycles;

   for (int i = 0; i < (int)rag_objs.size(); ++i) {
      tarjanAlgorithm(&rag_objs[i],cycles);
      rag_cycles.push_back(cycles);
      cycles.clear();
   }

   // .ragc files created for debugging purposes.
   // The vectors populated in this class will be used directly
   // in the deadlock data class. Since we have all the data in those vectors,
   // we do not need to parse the .ragc files.
   std::ofstream ofs;
   std::string rag_output_file_str = inp_file_prefix_str + "RAGC" + run_count_indicator_str + ".ragc";
   ofs.open(&rag_output_file_str[0]);

   for (int i = 0; i < (int)rag_cycles.size(); ++i) {
      if ((int)rag_cycles[i].size() > 0) {
         ofs << rag_timestamps[i] << "\n" << std::to_string((int)rag_cycles[i].size()) << "\n";
         for (int j = 0; j < (int)rag_cycles[i].size(); ++j) {
            ofs << rag_cycles[i][j] << "\n";
         }
         ofs << "\n";
      }
   }
   ofs.close();
}

void resultsAnalyzer::cleanUp() {
   // The inner most dimension represents a RAG, while the outermost dimension represents a cycle in RAG.
   // Each RAG can have more than one cycle, hence the 2D vector.
   std::vector<std::vector<std::string>>().swap(rag_cycles);
   // Input strings from unprocessed results file
   std::vector<std::string>().swap(input_strings);
   // Graph object representations of the above vector
   std::vector<graph>().swap(rag_objs);
   // Time points for the formation of each resource allocation graph in nanoseconds
   std::vector<uint64>().swap(rag_timestamps);
}

// Analysis object
resultsAnalyzer analyzer_obj;
