/*

Eric Wolfson
analysischart.cpp
The primary chart visualization (implementation)

*/

#include "analysischart.h"

barSegment::barSegment() {
   startx = endx = color = 0;
}

barSegment::barSegment(int sx,int ex,int col,std::string sstr,std::string estr) {
   startx = sx;
   endx = ex;
   color = col;
   start_str = sstr;
   end_str = estr;
}

int barSegment::getStartX() {
   return startx;
}

int barSegment::getEndX() {
   return endx;
}

int barSegment::getWidth() {
   return (endx - startx);
}

void barSegment::setStartX(int sx) {
   startx = sx;
}

void barSegment::setEndX(int ex) {
   endx = ex;
}

void barSegment::setColor(int col) {
   color = col;
}

void barSegment::setStartString(std::string sstr) {
   start_str = sstr;
}

void barSegment::setEndString(std::string estr) {
   end_str = estr;
}

int barSegment::getColor() {
   return color;
}

std::string barSegment::getStartString() {
   return start_str;
}

std::string barSegment::getEndString() {
   return end_str;
}

threadActivityBar::threadActivityBar() {
}

void threadActivityBar::addSegment(int sx, int ex, int col,std::string sstr, std::string estr) {
   segments.push_back(barSegment(sx,ex,col,sstr,estr));
}

void threadActivityBar::setSegmentStartX(int i, int sx) {
   segments[i].setStartX(sx);
}

void threadActivityBar::setSegmentEndX(int i, int ex) {
   segments[i].setEndX(ex);
}

void threadActivityBar::setSegmentColor(int i, int col) {
   segments[i].setColor(col);
}

void threadActivityBar::setSegmentStartString(int i, std::string sstr) {
   segments[i].setStartString(sstr);
}

void threadActivityBar::setSegmentEndString(int i, std::string estr) {
   segments[i].setEndString(estr);
}

void threadActivityBar::eraseSegment(int i) {
   segments.erase(segments.begin() + i);
}

int threadActivityBar::getSegmentStartX(int i) {
   return segments[i].getStartX();
}

int threadActivityBar::getSegmentEndX(int i) {
   return segments[i].getEndX();
}

int threadActivityBar::getSegmentWidth(int i) {
   return segments[i].getWidth();
}

int threadActivityBar::getNumSegments() {
   return (int)segments.size();
}

int threadActivityBar::getSegmentColor(int i) {
   return segments[i].getColor();
}

std::string threadActivityBar::getSegmentStartString(int i) {
   return segments[i].getStartString();
}

std::string threadActivityBar::getSegmentEndString(int i) {
   return segments[i].getEndString();
}

analysisChart::analysisChart(int x,int y,int w,int h) : Fl_Box(x,y,w,h,0) {
   bar_height = 15;
   temp_thread_quantity = 0;
   cursor_str = "No data to show.";
   currentx_cursor = -1;
   currenty_cursor = -1;
   show_tooltip = false;
}

// Clean up sub-vectors, then entire vector...
void analysisChart::cleanUpChartData() {
   for (int i = 0; i < (int)thread_charts.size(); ++i) {
        std::vector<threadActivityBar>().swap(thread_charts[i].thread_bars);
        std::vector<std::vector<unlock_display_type>>().swap(thread_charts[i].thread_bar_unlock_data);
   }
   std::vector<chart_data>().swap(thread_charts);
}

// Current overall position of mouse on the chart along x axis.
// Covers entire chart area, not just what is currently displayed.
int analysisChart::getCurrentXCursor() {
    return (Fl::event_x() - internal_scroller->x() + internal_scroller->hscrollbar.value());
}

// Current overall position of mouse on the chart along y axis.
// Covers entire chart area, not just what is currently displayed
int analysisChart::getCurrentYCursor() {
    return (Fl::event_y() - internal_scroller->y() + internal_scroller->scrollbar.value());
}

// Sets the height of the chart in pixels.
// Needed whenever a new chart is loaded and displayed.
void analysisChart::setChartHeight(int index) {
   h((int)std::max(352,bar_height*bar_space_factor*temp_thread_quantity));
}

void analysisChart::resizeThreadChartsVector(int sze) {
   std::vector<chart_data>().swap(thread_charts);
   thread_charts.resize(sze);
}

void analysisChart::setTempThreadQuantity(int ttq) {
   temp_thread_quantity = ttq;
}

// Store segment data including:
// 1) segment colors
// 2) segment text
// 3) segment positions
// ...for chart_data object "v" (index v in thread_charts)
void analysisChart::populateThreadActivityBarVector(int v) {
   int color = 0;
   int tindex = 0;
   int node_connect_val = -1;
   int next_node_connect_val = -1;
   int prev_node_connect_val = -1;
   int num_graphs = 0;
   bool deadlock_occurred = false;
   std::string start_str = "";
   std::string end_str = "";
   thread_charts[v].thread_bars.resize(temp_thread_quantity);
   for (int i = 0; i < temp_thread_quantity; ++i) {
        num_graphs = (int)pd_obj.displayed_graphs[v].size() - 1;
        thread_charts[v].thread_bars[i] = threadActivityBar();
        if (num_graphs > 1) {
            thread_charts[v].thread_bars[i].addSegment(0,pd_obj.bar_positions[v][1],GREEN_BAR_COLOR,"","");
        }
        for (int j = 1; j < num_graphs; ++j) {
             tindex = pd_obj.displayed_graphs[v][j].val2Index(NT_THREAD,i);
             node_connect_val = pd_obj.displayed_graphs[v][j].getNode(tindex)->val;
             next_node_connect_val = pd_obj.displayed_graphs[v][j+1].getNode(tindex)->val;
             prev_node_connect_val = pd_obj.displayed_graphs[v][j-1].getNode(tindex)->val;
             deadlock_occurred = deadlockOccurredAtThisPoint(v,i,j,num_graphs-1);
             // Deadlocks are red bars with a "DL: " string followed by a cycle information string
             // Wait-request intervals are orange bars followed by "I#" strings if lock instantly acquired or
             //                                                    "R#......A#" strings if not.
             // I stands for instant, R stands for request, A stands for acquire
             if (deadlock_occurred) {
                 start_str = "     DL: " + getDLStringOfThread(v,i,j,num_graphs-1);
                 end_str = "";
                 color = RED_BAR_COLOR;
             }
             else if (node_connect_val > -1 && next_node_connect_val > -1 && prev_node_connect_val == -1) {
                 start_str = "R" + std::to_string(node_connect_val);
                 end_str = "";
                 color = ORANGE_BAR_COLOR;
             }
             else if (node_connect_val > -1 && next_node_connect_val == -1 && prev_node_connect_val > -1) {
                 start_str = "A" + std::to_string(node_connect_val);
                 end_str = "";
                 color = GREEN_BAR_COLOR;
             }
             else if (next_node_connect_val > -1 && j == num_graphs - 1) {
                 start_str = "R" + std::to_string(next_node_connect_val);
                 end_str = "";
                 color = ORANGE_BAR_COLOR;
             }
             else if (node_connect_val > -1 && next_node_connect_val == -1 && prev_node_connect_val == -1) {
                 start_str = "I" + std::to_string(node_connect_val);
                 end_str = "";
                 color = GREEN_BAR_COLOR + 0x100;
             }
             else if (node_connect_val > -1) {
                 start_str = "R" + std::to_string(node_connect_val);
                 end_str = "A" + std::to_string(node_connect_val);
                 color = ORANGE_BAR_COLOR;
             }
             else {
                 start_str = "";
                 end_str = "";
                 color = GREEN_BAR_COLOR;
             }

             // Store segment data has been calculated.
             thread_charts[v].thread_bars[i].addSegment(pd_obj.bar_positions[v][j],pd_obj.bar_positions[v][j+1],color,start_str,end_str);
        }
   }

   // Merge same color segments that are consecutive
   // We do this that that the bars show as one continuous color (not split up),
   // while at the same time preserving the strings attached to the segments.
   for (int i = 0; i < temp_thread_quantity; ++i) {
        int seg = 1;
        while(seg < thread_charts[v].thread_bars[i].getNumSegments()) {
              if (thread_charts[v].thread_bars[i].getSegmentColor(seg-1) == thread_charts[v].thread_bars[i].getSegmentColor(seg)) {
                  thread_charts[v].thread_bars[i].setSegmentEndX(seg-1,thread_charts[v].thread_bars[i].getSegmentEndX(seg));
                  thread_charts[v].thread_bars[i].eraseSegment(seg);
              }
              else {
                  seg++;
              }
        }
   }
}

// Find unlock positions in analysis chart.
void analysisChart::populateThreadBarUnlockPositionsVector(int v) {
   if ((int)pd_obj.displayed_graphs[v].size() == 0) {
       return;
   }

   if ((int)thread_charts[v].thread_bars.size() == 0) {
       return;
   }

   int num_graphs = (int)pd_obj.displayed_graphs[v].size();
   int mindex = 0;
   int node_connect_val = -1;
   int prev_node_connect_val = -1;
   unlock_display_type udt;

   thread_charts[v].thread_bar_unlock_data.resize((int)thread_charts[v].thread_bars.size());

   for (int i = 0; i < (int)pd_obj.displayed_graphs[v][0].getNumMutexNodes(); ++i) {
        for (int j = 1; j < num_graphs; ++j) {
             mindex = pd_obj.displayed_graphs[v][j].val2Index(NT_MUTEX,i);
             node_connect_val = pd_obj.displayed_graphs[v][j].getNode(mindex)->val;
             prev_node_connect_val = pd_obj.displayed_graphs[v][j-1].getNode(mindex)->val;
             if (node_connect_val == -1 && prev_node_connect_val > -1) {
                 udt.mut_val = i;
                 udt.position = pd_obj.bar_positions[v][j];
                 udt.unlock_str = "U" + std::to_string(udt.mut_val);
                 thread_charts[v].thread_bar_unlock_data[prev_node_connect_val].push_back(udt);
             }
        }
   }
}

void analysisChart::setScroller(Fl_Scroll *scroll) {
   internal_scroller = scroll;
}

void analysisChart::setSlider(Fl_Hor_Slider *slider) {
   internal_slider = slider;
   internal_slider->when(FL_WHEN_CHANGED);
   internal_slider->callback(analysisChartSlider_CB,(void *)this);
   internal_slider->bounds(0,0);
   internal_slider->step(1);
   internal_slider->value(0);
   internal_slider->align(FL_ALIGN_INSIDE);
   internal_slider->label("");
}

Fl_Hor_Slider *analysisChart::getSlider() {
   return internal_slider;
}

chart_data *analysisChart::getThreadChart(int i) {
   return &(thread_charts[i]);
}

int analysisChart::getNumThreadCharts() {
   return (int)thread_charts.size();
}

// Override of virtual "handle" function
// for showing tooltip.
// Tooltip includes time in nanoseconds that the cursor
// is at along the thread bar, as well as which thread bar
// the mouse is hovering over.
int analysisChart::handle(int e) {
   switch (e) {
       case(FL_LEAVE): {
            show_tooltip = false;
            redraw();
            return 1;
       }
       case(FL_ENTER): {
            show_tooltip = true;
            redraw();
            return 1;
       }
       case(FL_MOVE): {
            show_tooltip = true;
            int cdi = pd_obj.ac_vector_index_value;
            if ((int)thread_charts.size() > 0) {
                if ((int)thread_charts[cdi].thread_bars.size() > 0) {
                    uint64 current_x = (uint64)getCurrentXCursor();
                    int current_y = getCurrentYCursor();
                    std::string ns_str = std::to_string(current_x*pd_obj.max_pos_val/(uint64)max_analysis_chart_bar_width) + " ns";
                    std::string th_str = "Thread " + std::to_string(current_y/(bar_height*bar_space_factor));
                    std::string total_str = ns_str + " " + th_str;
                    if (current_x < max_analysis_chart_bar_width && current_y < (bar_height*bar_space_factor*temp_thread_quantity)) {
                        cursor_str = total_str;
                    }
                    else {
                        cursor_str = "Mouse out of range.";
                    }
                }
                else {
                    cursor_str = "No thread-lock activity.";
                }
            }
            else {
                cursor_str = "No data to show.";
            }
            redraw();
            return 1;
       }
       default:
            break;
   }
   return(Fl_Box::handle(e));
}

// Override of virtual draw function:
// Displays the current chart.
void analysisChart::draw() {
   int bar_posx = 0;
   int bar_posy = 0;
   int cdi = pd_obj.ac_vector_index_value;
   Fl_Boxtype boxtype = FL_NO_BOX;
   std::string interval_start_str = "";
   std::string interval_end_str = "";
   std::string no_activity_str = "";
   std::string unlock_str = "";
   fl_draw_box(FL_FLAT_BOX,x(),y(),w(),h(),FL_BLACK);
   // no chart data loaded...
   if ((int)thread_charts.size() == 0) {
       fl_color(FL_WHITE);
       fl_font(FL_HELVETICA,12);
       fl_draw("There is no activity to display.",x()+4,y()+16);
   }
   // chart data shows no thread-lock activity
   else if ((int)thread_charts[cdi].thread_bars.size() == 0 && temp_thread_quantity > 0) {
       no_activity_str = "None of the " + std::to_string(temp_thread_quantity) + " threads requested or acquired a lock. All threads run to completion...";
       fl_color(FL_GREEN);
       fl_font(FL_HELVETICA,12);
       fl_draw(&no_activity_str[0],x()+4,y()+16);
   }
   // bars with text are drawn
   else {
       for (int i = 0; i < (int)thread_charts[cdi].thread_bars.size(); ++i) {
            bar_posy = y()+(i*bar_height*bar_space_factor);
            for (int j = 0; j < thread_charts[cdi].thread_bars[i].getNumSegments(); ++j) {
                 bar_posx = x()+thread_charts[cdi].thread_bars[i].getSegmentStartX(j);
                 boxtype = (thread_charts[cdi].thread_bars[i].getSegmentWidth(j) <= 15 ? FL_FLAT_BOX : FL_UP_BOX);
                 fl_draw_box(boxtype,bar_posx,bar_posy,thread_charts[cdi].thread_bars[i].getSegmentWidth(j),bar_height,
                                                       thread_charts[cdi].thread_bars[i].getSegmentColor(j));
                 interval_start_str = thread_charts[cdi].thread_bars[i].getSegmentStartString(j);
                 interval_end_str = thread_charts[cdi].thread_bars[i].getSegmentEndString(j);
                 fl_color(FL_WHITE);
                 fl_font(FL_HELVETICA,8);
                 fl_draw(&interval_start_str[0],bar_posx-12,bar_posy+(bar_height*bar_space_factor)-3);
                 fl_draw(&interval_end_str[0],bar_posx+6,bar_posy+(bar_height*bar_space_factor)-3);
                 if (j == thread_charts[cdi].thread_bars[i].getNumSegments() - 1 &&
                          thread_charts[cdi].thread_bars[i].getSegmentColor(j) == GREEN_BAR_COLOR) {
                     fl_draw("Thread Exits Successfully.",x()+max_analysis_chart_bar_width + 5,bar_posy+bar_height-3);
                 }
            }
            for (int u = 0; u < (int)thread_charts[cdi].thread_bar_unlock_data[i].size(); ++u) {
                 unlock_str = thread_charts[cdi].thread_bar_unlock_data[i][u].unlock_str;
                 fl_color(0x007fff00);
                 fl_font(FL_HELVETICA,8);
                 fl_draw(&unlock_str[0],x()+thread_charts[cdi].thread_bar_unlock_data[i][u].position-13,bar_posy+(bar_height*bar_space_factor)-5);
            }
       }
   }

   if (show_tooltip) {
       fl_draw_box(FL_EMBOSSED_BOX,x()+getCurrentXCursor(),y()+getCurrentYCursor()-12,(int)cursor_str.size()*6,12,FL_YELLOW);
       fl_color(FL_BLACK);
       fl_font(FL_HELVETICA,10);
       fl_draw(&cursor_str[0],x()+getCurrentXCursor()+4,y()+getCurrentYCursor()-2);
   }
}

// Helper function for threadInDeadlockInstance:
// Needed to take into account every last thread bar interval
bool deadlockOccurredAtThisPoint(int v, int th, int gi, int max_val) {
   if (gi < max_val) {
       return dlprocess_objs[v].threadInDeadlockInstance(th,gi);
   }
   return dlprocess_objs[v].threadInDeadlockInstance(th,gi+1);
}

// Helper function for getThreadDLInformationStr:
// Needed to take into account all thread bar intervals.
std::string getDLStringOfThread(int v, int th, int gi, int max_val) {
   if (gi < max_val) {
       return dlprocess_objs[v].getThreadDLInformationStr(&pd_obj,th,gi,v);
   }
   return dlprocess_objs[v].getThreadDLInformationStr(&pd_obj,th,gi+1,v);
}

// Pressing the blue "Load" button calls this function.
// Sets up the set of thread bar charts for each run.
// "Each run" refers to last time the green "Primary Execution Button" was pressed.
// *data is needed for fetching and using the primary analysisChart object
void analysisChartLoadButton_CB(Fl_Widget *w, void *data) {
   if (pd_obj.pipeline_running) {
       return;
   }
   if ((int)pd_obj.displayed_graphs.size() == 0) {
       return;
   }
   if (((Fl_Button *)w)->value() == 1) {
         int num_execution_runs = pd_obj.num_execution_runs;
         analysisChart * achart = (analysisChart *)data;
         if (achart->getNumThreadCharts() > 0) {
             achart->cleanUpChartData();
         }
         achart->getSlider()->bounds(0,num_execution_runs - 1);
         achart->getSlider()->step(1);
         achart->getSlider()->value(0);
         achart->resizeThreadChartsVector(num_execution_runs);
         achart->getSlider()->label("1");
         pd_obj.ac_vector_index_value = 0;
         for (int i = 0; i < num_execution_runs; ++i) {
              if ((int)pd_obj.displayed_graphs[0].size() >= 1) {
                   achart->setTempThreadQuantity(pd_obj.displayed_graphs[0][0].getNumThreadNodes());
              }
              if ((int)pd_obj.displayed_graphs[0].size() > 1) {
                   achart->populateThreadActivityBarVector(i);
                   achart->populateThreadBarUnlockPositionsVector(i);
                   achart->setChartHeight(0);
              }
         }
         achart->redraw();
   }
}
// Scan through the various charts for each run of the pipeline
// for the given input file.
void analysisChartSlider_CB(Fl_Widget *w, void *data) {
   analysisChart *achart = (analysisChart *)data;
   pd_obj.ac_vector_index_value = achart->getSlider()->value();
   std::string ch_label = std::to_string(pd_obj.ac_vector_index_value + 1);
   achart->getSlider()->copy_label(&ch_label[0]);
   achart->setChartHeight(pd_obj.ac_vector_index_value);
   achart->redraw();
}

