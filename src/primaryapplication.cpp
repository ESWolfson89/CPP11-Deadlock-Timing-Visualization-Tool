/*

Eric Wolfson
primaryapplication.cpp
Class implementation for primary window class

*/

#include "primaryapplication.h"

void initProgData() {
   pd_obj.inp_file_choice_str = "";
   pd_obj.num_execution_runs = 1;
   pd_obj.ac_vector_index_value = 0;
   pd_obj.num_input_files = 0;
   pd_obj.ci_flag = true;
   pd_obj.ri_flag = true;
   pd_obj.pipeline_running = false;
   pd_obj.max_pos_val = 1ULL;
   pd_obj.parameter_input_data[(int)IPT_PROGRAM_DURATION] = 1000;
   pd_obj.parameter_input_data[(int)IPT_NUM_MUTEXES] = 2;
   pd_obj.parameter_input_data[(int)IPT_NUM_THREADS] = 2;
   pd_obj.parameter_input_data[(int)IPT_MIN_SLEEPVAL] = 10;
   pd_obj.parameter_input_data[(int)IPT_MAX_SLEEPVAL] = 100;
   pd_obj.parameter_input_data[(int)IPT_INSTPAIRS_PERTHREAD] = 4;
   pd_obj.parameter_input_data[(int)IPT_LOCK_INSTCHANCE] = 50;
}

primaryApplicationClass::primaryApplicationClass(int w, int h) : Fl_Double_Window(w,h,0) {
   label("C++11 Deadlock Timing Visualization Tool (Eric Wolfson MSProject UIC Fall 2016/Spring 2017)");

   ecb_obj = new executionControlBox(0,32,384,192);

   chart_label = new Fl_Box(768,8,1,1,"THREAD ACTIVITY CHART");
   chart_label->align(FL_ALIGN_CENTER);

   chart_scroller = new Fl_Scroll(384,48,768,336);
   chart_scroller->begin();
   chart_obj = new analysisChart(384,48,max_analysis_chart_bar_width + 1000,336);
   chart_obj->setScroller(chart_scroller);
   chart_scroller->end();
   chart_slider = new Fl_Hor_Slider(384,18,704,30);
   chart_slider->color(0x6f6f6f00);
   chart_obj->setSlider(chart_slider);
   chart_refresh_button = new Fl_Button(1085,18,64,30,"Load");
   chart_refresh_button->color(0x007fff00);
   chart_refresh_button->when(FL_WHEN_CHANGED);
   chart_refresh_button->callback(analysisChartLoadButton_CB,chart_obj);

   icg_obj = new inputControlGroup(384,400,768,176,100,8);

   running_color_indicator = new Fl_Box(2,2,16,32,"Running");
   running_color_indicator->box(FL_UP_BOX);
   running_color_indicator->align(FL_ALIGN_RIGHT);
   running_color_indicator->color(GREEN_BAR_COLOR);

   waiting_color_indicator = new Fl_Box(83,2,16,32,"Waiting");
   waiting_color_indicator->box(FL_UP_BOX);
   waiting_color_indicator->align(FL_ALIGN_RIGHT);
   waiting_color_indicator->color(ORANGE_BAR_COLOR);

   deadlock_color_indicator = new Fl_Box(160,2,16,32,"Deadlock");
   deadlock_color_indicator->box(FL_UP_BOX);
   deadlock_color_indicator->align(FL_ALIGN_RIGHT);
   deadlock_color_indicator->color(RED_BAR_COLOR);

   text_indicator1 = new Fl_Box(248,4,1,1,"(I)nstant (R)equest (A)cquire lock");
   text_indicator1->labelsize(8);
   text_indicator1->align(FL_ALIGN_RIGHT);
   text_indicator1->box(FL_NO_BOX);

   text_indicator2 = new Fl_Box(248,15,1,1,"DL (cycle): Deadlock (cycle info)");
   text_indicator2->labelsize(8);
   text_indicator2->align(FL_ALIGN_RIGHT);
   text_indicator2->box(FL_NO_BOX);

   text_indicator3 = new Fl_Box(248,26,1,1,"U(num): Unlock mutex (num)");
   text_indicator3->labelsize(8);
   text_indicator3->align(FL_ALIGN_RIGHT);
   text_indicator3->box(FL_NO_BOX);

   show();
}


