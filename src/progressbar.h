/*

Eric Wolfson
progressbar.h
Indicates execution pipelines progress via colored blue bar

*/


#ifndef PROGRESSBAR_H_
#define PROGRESSBAR_H_

#include "fltkincludes.h"

// Blue progress bar to indicate how complete the pipeline run is (or set of runs are).
class executionProgressBar : public Fl_Box {
   public:
      executionProgressBar(int,int,int,int);
      void setProgressRatio(double);
      void draw();
   private:
      double progress_ratio;
};

#endif
