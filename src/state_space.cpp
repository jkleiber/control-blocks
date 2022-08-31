#include "controlblocks/state_space.h"

namespace ControlUtils
{
    Eigen::VectorXd StateSpace::UpdateDynamics(Eigen::VectorXd x,
                                               Eigen::VectorXd u)
    {
        // Ensure A,B,C,D match x,u dimensions
        bool A_match = (A_.rows() == x.size()) && (A_.cols() == x.size());
        bool B_match = (B_.rows() == x.size()) && (B_.cols() == u.size());

        // Throw an exception for the user to figure out
        // where they went wrong.
        if (!(A_match && B_match))
        {
            throw std::runtime_error(
                "SS update matrices dimensions do not commute! A: " +
                std::to_string(A_match) + " B: " + std::to_string(B_match));
        }

        // Update dx
        Eigen::VectorXd dx = A_ * x + B_ * u;
        return dx;
    }

    Eigen::VectorXd StateSpace::GetOutput(Eigen::VectorXd x, Eigen::VectorXd u)
    {
        bool C_match = (C_.cols() == x.size());
        bool D_match = D_.cols() == u.size();

        // Throw an exception for the user to figure out
        // where they went wrong.
        if (!(C_match && D_match && (C_.rows() == D_.rows())))
        {
            throw std::runtime_error(
                "SS output matrices dimensions do not commute! C: " +
                std::to_string(C_match) + " D: " + std::to_string(D_match) +
                " C and D: " + std::to_string(C_.rows() == D_.rows()));
        }

        // Compute the output, y
        Eigen::VectorXd y = C_ * x + D_ * u;
        return y;
    }

    int StateSpace::NumInputs() { return B_.cols(); }
    int StateSpace::NumOutputs() { return C_.rows(); }
    int StateSpace::NumStates() { return A_.rows(); }

    void StateSpace::SetABCD(Eigen::MatrixXd A, Eigen::MatrixXd B,
                             Eigen::MatrixXd C, Eigen::MatrixXd D)
    {
        A_ = A;
        B_ = B;
        C_ = C;
        D_ = D;
    }
} // namespace ControlUtils