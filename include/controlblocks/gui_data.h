
#pragma once

typedef struct gui_data_t
{
    double sim_time = 10.0;
    double dt = 0.1;
    bool start;
    bool pause;
    bool stop;
} GuiData;
