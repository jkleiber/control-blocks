#include "controlblocks/vector_utils.h"
#include <iostream>

namespace ControlUtils
{
    Eigen::VectorXd StackVectors(std::vector<Eigen::VectorXd> vectors)
    {
        Eigen::VectorXd output = Eigen::VectorXd::Zero(1);
        int idx = 0;
        for (size_t i = 0; i < vectors.size(); ++i)
        {
            // Get size increase
            int size_delta = vectors[i].size();

            // Resize
            output.conservativeResize(idx + size_delta);

            // Add the data
            output.segment(idx, vectors[i].size()) = vectors[i];
            idx += size_delta;
        }

        return output;
    }
} // namespace ControlUtils
