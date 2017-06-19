/*

Eric Wolfson
main.cpp
Entry point cpp file for program for MS Project UIC Fall 2016-Spring 2017

A Tool for analyzing deadlock timing conditions for simple C++11 programs following
a specified pattern.

REQUIREMENTS:
-) C++ 11 (for main application)
-) g++5 (earlier versions may not yield correct return values for "try_lock_until" function
   in generated programs
-) FLTK 1.3.4 widgets library

*/

#include "primaryapplication.h"

int main() {
    // Initialize pd_obj (global_prog_data) fields
    initProgData();
    // Declare instance of main program class
    primaryApplicationClass pac_obj(1152,576);
    // This program is event-driven. Using FLTK
    // means we must call Fl::run() to run the window application.
    // "Fl::run()" does not return until the window is closed.
    // This is the event loop.
    return(Fl::run());
}

