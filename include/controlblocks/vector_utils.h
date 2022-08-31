
#pragma once

#include <vector>

#include <Eigen/Dense>

namespace ControlUtils
{
    Eigen::VectorXd StackVectors(std::vector<Eigen::VectorXd> vectors);
}