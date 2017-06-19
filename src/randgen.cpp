/*

Eric Wolfson
randgen.cpp
random number generator function implementation

*/

#include "randgen.h"

// This functon returns a random non-negative integer between
// and possibly including the two input parameters.
// i.e. [low,high] where low is lower input parameter, and high is higher input parameter
int randInt(int low, int high) {
    if (high <= low)
        return high;

    // make all values have equal chance of being selected
    std::uniform_int_distribution<int> dist(low,high);
    return dist(random_number_generator);
}

// Returns true with a "percentage" percent chance.
bool rollPercentage(int percentage) {
    return (randInt(0,99) < percentage);
}


