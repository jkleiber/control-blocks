
#pragma once

#include <memory>
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
        std::shared_ptr<Port> GetInputPort(int index);
        int GetInputPortId(int index);
        std::shared_ptr<Port> FindInputPortByImNodesId(int imnode_id);

        int NumOutputPorts();
        std::shared_ptr<Port> GetOutputPort(int index);
        int GetOutputPortId(int index);
        std::shared_ptr<Port> FindOutputPortByImNodesId(int imnode_id);

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
        std::vector<std::shared_ptr<Port>> inputs_;
        std::vector<std::shared_ptr<Port>> outputs_;
        std::vector<int> input_ids_;
        std::vector<int> output_ids_;
    };
} // namespace ControlBlock
