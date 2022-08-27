
#pragma once

#include <iostream>

#include <Eigen/Dense>
#include <toml++/toml.h>

#include "controlblocks/block.h"

namespace ControlBlock
{

    class GainBlock : public Block
    {
    public:
        GainBlock(Diagram &diagram)
            : Block(diagram), val_(0.0), min_node_width_(50.0)
        {
        }

        void Init(std::string block_name = "Gain");
        void ApplyInitial() override;
        void SetInitial(Eigen::VectorXd x0) override;
        void Compute() override;
        void Render() override;

        // Serialization
        toml::table Serialize() override;
        void Deserialize(toml::table data) override;

    private:
        double val_;
        Eigen::VectorXd x0_;

        std::string input_port_name_;
        std::string output_port_name_;

        const float min_node_width_;
    };

} // namespace ControlBlock
