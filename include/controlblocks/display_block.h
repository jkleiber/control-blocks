
#pragma once

#include <iostream>

#include <Eigen/Dense>
#include <toml++/toml.h>

#include "controlblocks/block.h"

namespace ControlBlock
{

    class DisplayBlock : public Block
    {
    public:
        DisplayBlock(Diagram &diagram)
            : Block(diagram), val_(0), min_node_width_(50.0)
        {
        }

        void Init(std::string block_name = "Display");
        void Compute(double t) override;
        void Render() override;

        // Serialization
        toml::table Serialize() override;

    private:
        Eigen::VectorXd val_;

        std::string input_port_name_;
        std::string output_port_name_;

        const float min_node_width_;
    };

} // namespace ControlBlock
