
#pragma once

#include <iostream>

#include <Eigen/Dense>
#include <toml++/toml.h>

#include "controlblocks/block.h"

namespace ControlBlock
{

    class MuxBlock : public Block
    {
    public:
        MuxBlock(Diagram &diagram)
            : Block(diagram), num_mux_inputs(2), min_node_width_(50.0),
              settings_open_(false)
        {
        }

        void Init(std::string block_name = "Mux");
        void Compute() override;
        void Render() override;
        void Settings() override;

        // Serialization
        toml::table Serialize() override;
        void Deserialize(toml::table data) override;

    private:
        int num_mux_inputs;

        std::string output_port_name_;

        const float min_node_width_;

        bool settings_open_;
    };

} // namespace ControlBlock
