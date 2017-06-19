/*

Eric Wolfson
processdeadlock.cpp
Class implementation for processDeadlock class
Contains deadlock information 

*/

#include "processdeadlock.h"

processDeadlock::processDeadlock() {
}

// Get string in the form of "DL: " + ret_val
// ...where ret_val is cycle in RAG
std::string processDeadlock::getThreadDLInformationStr(global_prog_data *pd_param, int th_param, int gi, int v) {
   std::string ret_val = "";
   int dl_index = -1;

   int start_node_index = -1;
   int current_node_index = -1;
   int current_node_val = -1;
   int iter = 1;

   // Check to see which index "i" in "detected_deadlocks" contains this thread's id (th_param) at timestamp
   // "gi" (graph index)
   for (int i = 0; i < (int)detected_deadlocks.size(); ++i) {
        if (detected_deadlocks[i].dl_timestamp == rag_timestamps[gi]) {
              if(std::find(detected_deadlocks[i].dl_thread_ids.begin(), detected_deadlocks[i].dl_thread_ids.end(), th_param) !=
                           detected_deadlocks[i].dl_thread_ids.end()) {
               dl_index = i;
            }
        }
   }

   // ret_val bcomes string representing cycle. For example:
   // "T0->M0->T1->M1->T2->M2->T0"
   if (dl_index > -1) {
       current_node_index = start_node_index = pd_param->displayed_graphs[v][gi].val2Index(NT_THREAD,th_param);
       ret_val += "T" + std::to_string(th_param) + "->";
       do {
            current_node_val = pd_param->displayed_graphs[v][gi].getNode(current_node_index)->val;
            if (iter % 2 == 1) {
                ret_val += "M" + std::to_string(current_node_val) + "->";
                current_node_index = pd_param->displayed_graphs[v][gi].val2Index(NT_MUTEX,current_node_val);
            }
            else {
                ret_val += "T" + std::to_string(current_node_val) + "->";
                current_node_index = pd_param->displayed_graphs[v][gi].val2Index(NT_THREAD,current_node_val);
            }
            iter++;
       } while(current_node_index != start_node_index);
   }

   // Get rid of final hanging arrow...
   ret_val.pop_back();
   ret_val.pop_back();

   return ret_val;
}

// Needed for determining if a deadlock occurred at thread th, in RAG gi.
bool processDeadlock::threadInDeadlockInstance(int th, int gi) {
   for (int i = 0; i < (int)detected_deadlocks.size(); ++i) {
        if (detected_deadlocks[i].dl_timestamp == rag_timestamps[gi]) {
              if(std::find(detected_deadlocks[i].dl_thread_ids.begin(), detected_deadlocks[i].dl_thread_ids.end(), th) !=
                           detected_deadlocks[i].dl_thread_ids.end()) {
                 return true;
              }
        }
   }

   return false;
}

int processDeadlock::getNumDetectedDeadlocks() {
   return (int)detected_deadlocks.size();
}

int processDeadlock::getMutexID(int i, int j) {
   return detected_deadlocks[i].dl_mutex_ids[j];
}

int processDeadlock::getThreadID(int i, int j) {
   return detected_deadlocks[i].dl_thread_ids[j];
}

int processDeadlock::getDLTimestampIndex(int i) {
   return detected_deadlocks[i].dl_timestamp_index;
}

uint64 processDeadlock::getDLTimestamp(int i) {
   return detected_deadlocks[i].dl_timestamp;
}

uint64 processDeadlock::getRAGTimestamp(int i) {
   return rag_timestamps[i];
}

void processDeadlock::cleanUp() {
   std::vector<deadlock_type>().swap(detected_deadlocks);
   std::vector<uint64>().swap(rag_timestamps);
}

// Populate deadlock data vector
void processDeadlock::populateDeadlockVector(resultsAnalyzer *r_analyzer_ptr) {
   deadlock_type dt = {0ULL,0,std::vector<int>(),
                              std::vector<int>()};

   std::string current_rag_str = "";
   std::string current_id_str = "";
   bool th_flag;
   for (int i = 0; i < r_analyzer_ptr->getRagTimestampVectorSize(); ++i) {
        rag_timestamps.push_back(r_analyzer_ptr->getRagTimestamp(i));
   }
   for (int i = 0; i < r_analyzer_ptr->getRagCyclesVectorSize(); ++i) {
        for (int j = 0; j < r_analyzer_ptr->getRagCyclesVectorSizeForTimestamp(i); ++j) {
             current_rag_str = r_analyzer_ptr->getRagCycle(i,j);
             dt.dl_thread_ids.clear();
             dt.dl_mutex_ids.clear();
             dt.dl_timestamp = r_analyzer_ptr->getRagTimestamp(i);
             dt.dl_timestamp_index = j;
             for (int c = 0; c < (int)current_rag_str.size(); ++c) {
                  switch (current_rag_str[c]) {
                       case('T'): {
                            th_flag = true;
                            break;
                       }
                       case('M'): {
                            th_flag = false;
                            break;
                       }
                       case(' '): {
                            if (th_flag) {
                                dt.dl_thread_ids.push_back(std::stoi(current_id_str));
                            }
                            else {
                                dt.dl_mutex_ids.push_back(std::stoi(current_id_str));
                            }
                            current_id_str = "";
                            break;
                       }
                       case('0'): case('1'): case('2'): case('3'): case('4'):
                       case('5'): case('6'): case('7'): case('8'): case('9'): {
                            current_id_str += current_rag_str[c];
                            break;
                       }
                       default:
                            break;
                  }
             }
             detected_deadlocks.push_back(dt);
        }
   }
}

// Set of deadlock data objects
std::vector<processDeadlock> dlprocess_objs;
