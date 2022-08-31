
#pragma once

#include <iostream>

#include <Eigen/Dense>

namespace ControlUtils
{
    class StateSpace
    {
    public:
        StateSpace() {}
        StateSpace(Eigen::MatrixXd A, Eigen::MatrixXd B, Eigen::MatrixXd C,
                   Eigen::MatrixXd D)
            : A_(A), B_(B), C_(C), D_(D)
        {
        }

        Eigen::VectorXd UpdateDynamics(Eigen::VectorXd x, Eigen::VectorXd u);
        Eigen::VectorXd GetOutput(Eigen::VectorXd x, Eigen::VectorXd u);

        // Get the dimensions
        int NumInputs();
        int NumOutputs();
        int NumStates();

        // Set the matrices
        void SetABCD(Eigen::MatrixXd A, Eigen::MatrixXd B, Eigen::MatrixXd C,
                     Eigen::MatrixXd D);

    private:
        // State space
        Eigen::MatrixXd A_, B_, C_, D_;
    };
} // namespace ControlUtils