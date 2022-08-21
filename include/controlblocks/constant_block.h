
#pragma once

#include <iostream>

#include <Eigen/Dense>

#include "controlblocks/block.h"

namespace ControlBlock
{

    class ConstantBlock : public Block
    {
    public:
        ConstantBlock(Diagram &diagram)
            : Block(diagram), val_(0.0), min_node_width_(50.0)
        {
        }

        void Init(std::string block_name = "Constant");
        void Compute() override;
        void Render() override;

    private:
        double val_;

        std::string output_port_name_;

        const float min_node_width_;
    };

} // namespace ControlBlock
