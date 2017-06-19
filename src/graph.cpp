/*

Eric Wolfson
graph.cpp
graph class implementation

*/

#include "graph.h"

graph::graph() {
   num_total_nodes = 0;
   num_mutex_nodes = 0;
   num_thread_nodes = 0;
}

graph::graph(int num_mutexes, int num_threads) {
   initGraph(num_mutexes,num_threads);
}

node *graph::getNode(int i) {
   return &(all_nodes[i]);
}

// Converts a node value to an index.
// Its just the input parameter unless the node is a thread (not a mutex).
// In which case we need to offset (logic described above)
int graph::val2Index(node_type nt, int i) {
   return (nt == NT_MUTEX ? i : i + num_mutex_nodes);
}

void graph::setNodeSCCIndex(int i, int scc_index) {
   all_nodes[i].scc_index = scc_index;
}

void graph::setNodeSCCLowlink(int i, int scc_lowlink) {
   all_nodes[i].scc_lowlink = scc_lowlink;
}

void graph::setNodeSCCOnStack(int i, bool scc_onstack) {
   all_nodes[i].scc_onstack = scc_onstack;
}

void graph::cleanupGraph() {
   std::vector<node>().swap(all_nodes);
}

void graph::initGraph(int num_mutexes, int num_threads) {
   num_mutex_nodes = num_mutexes;
   num_thread_nodes = num_threads;
   num_total_nodes = num_mutex_nodes + num_thread_nodes;
   all_nodes.resize(num_total_nodes);
   // Initialize all nodes to lone nodes, but make sure first "num_mutex_nodes"
   // nodes are mutex nodes, while the remaining are thread nodes. Also store initial temp data
   // needed for Tarjan's Algorithm.
   for (int i = 0; i < num_total_nodes; ++i) {
      all_nodes[i] = {-1,(i < num_mutex_nodes ? NT_MUTEX : NT_THREAD),-1,-1,false,i};
   }
}

// Sets a nodes "val" field (what its connected to)
void graph::addEdge(node_type nt, int outgoing_val, int incoming_val) {
   int index = (nt == NT_MUTEX ? outgoing_val : outgoing_val + num_mutex_nodes);
   all_nodes[index].val = incoming_val;
}

int graph::getNumMutexNodes() {
   return num_mutex_nodes;
}

int graph::getNumThreadNodes() {
   return num_thread_nodes;
}

int graph::getNumTotalNodes() {
   return num_total_nodes;
}

// As described above, this function:
// Parses through a RAG represented as a string of the form:
// "T0-M#, T1-M#, T2-M#, ... , TK-M#, M0-T#, M1-T#, M2-T#, ... , ML-T#"
// for K-1 threads and L-1 mutexes
// Then stores this RAG string in "graph" form, where a graph is the class defined above.
void addAllEdges(graph *g, std::string s) {
   s += " ";
   node_type current_nt;
   int outgoing_val = 0;
   int incoming_val = -1;
   bool dash_flag = false;
   bool nullval_flag = false;
   std::string temp_val = "";
   for (int i = 0; i < (int)s.size(); ++i) {
      switch(s[i]) {
         case('T'):
            if (!dash_flag)
               current_nt = NT_THREAD;
            break;
         case('M'):
            if (!dash_flag)
               current_nt = NT_MUTEX;
            break;
         case('N'):
            incoming_val = -1;
            nullval_flag = true;
            break;
         case('-'):
            outgoing_val = std::stoi(temp_val);
            temp_val = "";
            dash_flag = true;
            break;
         case(' '):
            if (!nullval_flag) {
               incoming_val = std::stoi(temp_val);
               temp_val = "";
            }
            dash_flag = false;
            g->addEdge(current_nt,outgoing_val,incoming_val);
            break;
         case('0'):
         case('1'):
         case('2'):
         case('3'):
         case('4'):
         case('5'):
         case('6'):
         case('7'):
         case('8'):
         case('9'):
            nullval_flag = false;
            temp_val += s[i];
            break; 
         default:
            break;
      }
   }
}

