# control-blocks

The goal of this project is to make a node based control system block diagram software. 
This will enable open source development and simulation of control systems.

![Diagram Example](/img/example_diagram.png)

## Current Capability
- Basic GUI and simulation for simple math / constants
- Block types:
    - Constant
    - Sum
    - Gain (only faciliator of feedback at the moment)
    - Mux (can add ports but not remove)
    - Display
- Can add/remove blocks and wires
- Saving and loading the diagram (only one filename supported right now)

## Dependencies
See `third_party` for a list of dependencies and how to install them.


## Active Work
- ODE solver
    - Using Boost for this.
- Improve feedback systems
    - This should be ready theoretically if each Dynamic system block actually implements `ApplyInitial()`
    - Unclear if basic blocks like sum and mux should have this feature, but it is likely they should
- Variables in a workspace

## Roadmap
The current work is focusing on Steps 1-3.

1. Basic GUI and scripting
    - Scripting will tentatively be in python, but might end up in C++
    - C++ will be used in the backend for speed optimization in the future.
2. Simulation for general dynamical systems (x_dot = f(x,u,t))
    - Series simulation only, no summing junctions
3. Simulation for linear time invariant (LTI) systems
    - State-space LTI systems
    - S-domain transfer functions
4. Diagnostics for LTI systems
    - Bode plot
    - Nichols chart
    - Nyquist plot
    - Pole-zero map
    - Step response
    - Impulse response
5. Basic block diagram algebra
    - Summing junctions
    - Feedback systems
    - Multiplexing
6. Advanced block diagram manipulation
    - I/O linearization
7. Controllers
    - PID
    - LQR
    f
8. Advanced control 
    - H-infinity
    - Model predictive control (MPC)