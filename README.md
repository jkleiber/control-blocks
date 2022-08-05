# control-blocks

The goal of this project is to make a node based control system block diagram software. 
This will enable open source development and simulation of control systems.

## Roadmap

1. Basic GUI and scripting
    - Scripting will be in python
    - Rust or C++ will be used in the backend for speed optimization in the future.
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