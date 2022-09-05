
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
            : Block(diagram), min_node_width_(50.0)
        {
        }

        void Init(std::string block_name = "State Space");

        // Overriden Block functions
        void ApplyInitial() override;
        void Compute(double t) override;
        void Render() override;

        // Serialization
        toml::table Serialize() override;
        void Deserialize(toml::table data) override;

    private:
        ControlUtils::StateSpace ss;

        const float min_node_width_;
    };

} // namespace ControlBlock
