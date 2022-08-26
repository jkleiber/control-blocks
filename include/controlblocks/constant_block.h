
#pragma once

#include <iostream>

#include <Eigen/Dense>
#include <toml++/toml.h>

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
        void Load(toml::table block_table);
        void Compute() override;
        void Render() override;

        // Serialization
        toml::table Serialize() override;

    private:
        double val_;

        std::string output_port_name_;

        const float min_node_width_;
    };

} // namespace ControlBlock
