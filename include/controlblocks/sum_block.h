
#pragma once

#include <iostream>

#include <Eigen/Dense>
#include <toml++/toml.h>

#include "controlblocks/block.h"

namespace ControlBlock
{

    class SumBlock : public Block
    {
    public:
        SumBlock(Diagram &diagram) : Block(diagram), min_node_width_(50.0) {}

        void Init(std::string block_name = "Sum");
        void Compute(double t) override;
        void Render() override;

        // Serialization
        toml::table Serialize() override;

    private:
        std::string input1_, input2_;
        std::string output_port_name_;

        const float min_node_width_;
    };

} // namespace ControlBlock
