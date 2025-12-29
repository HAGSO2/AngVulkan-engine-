#pragma once
#include "defines.h"

class clock{
    f64 start_time;
    f64 elapsed;
    public:
    clock();

    // Updates the provided clock. Should be called just before checking elapsed time.
    // Has no effect on non-started clocks.
    f64 clock_update();

    // Starts the provided clock. Resets elapsed time.
    void clock_start();

    // Stops the provided clock. Does not reset elapsed time.
    void clock_stop();
};