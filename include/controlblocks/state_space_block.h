
#pragma once

#include <iostream>

#include <Eigen/Dense>
#include <toml++/toml.h>

#include "controlblocks/block.h"
#include "controlblocks/state_space.h"

namespace ControlBlock
{

    class StateSpaceBlock : public Block
    {
    public:
        StateSpaceBlock(Diagram &diagram)
            : Block(diagram), min_node_width_(50.0), settings_open_(true)
        {
        }

        void Init(std::string block_name = "State Space");

        // Overriden Block functions
        void ApplyInitial() override;
        void Compute(double t) override;
        void Render() override;
        void Settings() override;

        // Serialization
        toml::table Serialize() override;
        void Deserialize(toml::table data) override;

    private:
        ControlUtils::StateSpace ss;

        const float min_node_width_;

        bool settings_open_;

        // Matrices
        Eigen::MatrixXd A_;
        Eigen::MatrixXd B_;
        Eigen::MatrixXd C_;
        Eigen::MatrixXd D_;

        // Matrix names
        std::string A_mat_str_;
        std::string B_mat_str_;
        std::string C_mat_str_;
        std::string D_mat_str_;
    };

} // namespace ControlBlock
