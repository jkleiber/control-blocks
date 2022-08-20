
#pragma once

#include <string>
#include <vector>

#include "imgui.h"
#include "imnodes.h"
#include <Eigen/Dense>

#include "controlblocks/port.h"

class Diagram;

namespace ControlBlock
{
    class Block
    {
    public:
        Block(Diagram &diagram) : diagram_(diagram) {}

        void Init(std::string block_name, std::vector<std::string> input_names,
                  std::vector<std::string> output_names);
        void Broadcast();
        virtual void Compute();
        virtual void Render();

        // Block characteristics
        int GetId();
        std::string GetName();
        void SetPosition(const ImVec2 pos);

        // Ports
        int NumInputPorts();
        Port GetInputPort(int index);
        int GetInputPortId(int index);

        int NumOutputPorts();
        Port GetOutputPort(int index);
        int GetOutputPortId(int index);

        // Inputs
        Eigen::VectorXd GetInput(std::string port_name);

        // Outputs
        void SetOutput(std::string port_name, Eigen::VectorXd val);

    protected:
        // Block characteristics
        int id_;
        std::string name_;

        // Diagram membership
        Diagram &diagram_;

        // Block ports
        std::vector<Port> inputs_;
        std::vector<Port> outputs_;
        std::vector<int> input_ids_;
        std::vector<int> output_ids_;
    };
} // namespace ControlBlock
