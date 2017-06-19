/*

Eric Wolfson
randgen.h
Random number generator functions

*/

#ifndef RANDGEN_H_
#define RANDGEN_H_

#include "includes.h"

// Mersenne Twister random number generator built into C++11 is used
// for creating random input parameters.
static std::mt19937 random_number_generator(time(0));

// This functon returns a random non-negative integer between
// and possibly including the two input parameters.
// i.e. [a,b] where a is lower input parameter, and b is higher input parameter
int randInt(int,int);

// Call the parameter "percentage; returns true with a "percentage" percent chance.
bool rollPercentage(int);

#endif
