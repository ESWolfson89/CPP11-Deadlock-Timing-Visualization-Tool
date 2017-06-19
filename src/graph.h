/*

Eric Wolfson
graph.h
graph class declaration

*/

#ifndef GRAPH_H_
#define GRAPH_H_

#include "includes.h"

// Nodes in resource allocation graph can refer to a thread or a mutex.
enum node_type {NT_MUTEX, NT_THREAD};

struct node {
  // value of connected node (directed to)
  // For instance, if M5 then val == 5 if T5 then val = 5 + num_mutex_nodes
  int val;
  // a thread node cannot be connected to another thread node (and same
  // for mutex nodes), so the connected-to n_type is implied to be the opposite
  // of n_type.
  node_type n_type;
  // only used for computing cycles
  int scc_index;
  int scc_lowlink;
  bool scc_onstack;
  // raw index in vector
  int vec_index;
};

// Contains data about a resource allocation graph.
// Each node can have multiple directed incoming edges, but only at most one directed outgoing edge.
// Since this is the case we can store each node in a simple array, and this makes
// Tarjan's algorithm much simpler.
// The reasons for not needing multiple outgoing edges are because:
// 1) Each mutex can only be held by one thread at most.
// 2) Each thread can only be requesting one mutex at most.
// Outward edges from thread nodes indicate which mutex the thread is requesting.
// Outward edges from mutex nodes indicate which thread the mutex belongs to.
class graph {
  public:
     graph();
     graph(int,int);
     node *getNode(int);
     // The next 3 functions are Needed for computing Tarjan's Strongly Connected Components Algorithm.
     // They are temp variables needed for intermediate computations throughout the algorithm.
     void setNodeSCCIndex(int,int);
     void setNodeSCCLowlink(int,int);
     void setNodeSCCOnStack(int,bool);
     void cleanupGraph();
     // Connect two nodes first parameter directed to second parameter
     void addEdge(node_type,int,int);
     // Initialize number of threads and mutexes, as well as initial value of each node's
     // directed outward node (-1 in this case, so that all nodes are lone nodes).
     void initGraph(int,int);
     int getNumMutexNodes();
     int getNumThreadNodes();
     int getNumTotalNodes();
     // The first "num_mutexes_nodes" nodes represent mutexes
     // The remaining nodes represent threads for example if all_nodes.size() == 10
     // with 4 mutex nodes and 6 threads, the structure of all_nodes (field declared below)
     // all_nodes[0..5] -> value of 0 to 3 (or -1 if lone node)
     // all_nodes[6..9] -> value of 0 to 5 (or -1 of lone node)
     int val2Index(node_type,int);
  private:
     // Since no nodes can have more than 1 outgoing edge, we can
     // represent entire graph as 1D array, each value corresponding an
     // outgoing node (0..n) or -1 if no outgoing nodes.
     std::vector<node> all_nodes;
     int num_mutex_nodes;
     int num_thread_nodes;
     int num_total_nodes;
};

// Parses through a RAG represented as a string of the form:
// "T0-M#, T1-M#, T2-M#, ... , TK-M#, M0-T#, M1-T#, M2-T#, ... , ML-T#"
// for K-1 threads and L-1 mutexes
// Then stores this RAG string in "graph" form, where a graph is declared above.
void addAllEdges(graph *, std::string);

#endif
