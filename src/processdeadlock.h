/*

Eric Wolfson
processdeadlock.h
Class definition for processDeadlock class
Contains deadlock information 

*/

#ifndef PROCESSDEADLOCK_H_
#define PROCESSDEADLOCK_H_

#include "globalprogdata.h"
#include "resultsanalyzer.h"

// Deadlock data is stored in a "deadlock type" consists of...
struct deadlock_type {
   // Time at which deadlock occurred.
   uint64 dl_timestamp;
   // Index of RAG vector that deadlock occurred at
   int dl_timestamp_index;
   // Mutex nodes involved in deadlock
   std::vector<int> dl_mutex_ids;
   // Thread nodes involved in deadlock
   std::vector<int> dl_thread_ids;
};

// Parses through cycles found in resultsAnalyzer file
// to pinpoint deadlock conditions and store them in a way
// that can be easily displayed by the analysisChart class...
// This class analyzes the .ragc files.

// Process does not refer to instance of running program,
// but rather "to process" (avoiding confusion)
class processDeadlock {
   public:
      processDeadlock();
      void populateDeadlockVector(resultsAnalyzer *);
      void cleanUp();
      bool threadInDeadlockInstance(int,int);
      int getNumDetectedDeadlocks();
      int getMutexID(int,int);
      int getThreadID(int,int);
      int getDLTimestampIndex(int);
      uint64 getDLTimestamp(int);
      uint64 getRAGTimestamp(int);
      std::string getThreadDLInformationStr(global_prog_data *,int,int,int);
   private:
      std::vector<deadlock_type> detected_deadlocks;
      std::vector<uint64> rag_timestamps;
};

// Set of deadlock data objects
extern std::vector<processDeadlock> dlprocess_objs;

#endif
