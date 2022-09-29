
#pragma once

#include <string>
#include <vector>

static const std::vector<std::string> ode_solvers = {"RK4", "Cash-Karp54",
                                                     "dopri5"};

typedef struct gui_data_t
{
    // Timing
    double sim_time = 10.0;
    double dt = 0.1;

    // Events
    bool start;
    bool pause;
    bool stop;

    // ODE solver
    std::string solver = ode_solvers[0];

} GuiData;
