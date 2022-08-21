
#pragma once

typedef struct gui_data_t
{
    // Timing
    double sim_time = 10.0;
    double dt = 0.1;

    // Events
    bool start;
    bool pause;
    bool stop;
} GuiData;
