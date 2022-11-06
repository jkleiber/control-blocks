
#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "imgui.h"
#include "imnodes.h"
#include "toml++/toml.h"
#include <Eigen/Dense>

#include "controlblocks/code_tools.h"
#include "controlblocks/port.h"
#include "controlblocks/serializable.h"

class Diagram;
class Workspace;

namespace ControlBlock
{
    class Block : public Serializable
    {

    public:
        Block(Diagram &diagram) : diagram_(diagram) {}
        ~Block() {}

        void Init(std::string block_name, std::vector<std::string> input_names,
                  std::vector<std::string> output_names,
                  std::vector<bool> input_optionals = std::vector<bool>(),
                  bool dynamic_sys = false);
        void Broadcast();
        virtual bool ApplyInitial();
        virtual void Compute(double t);
        virtual void Render();
        virtual void Settings();

        // Dynamics
        virtual void SetInitial(Eigen::VectorXd x0);
        virtual bool GetDx(Eigen::VectorXd *dx);
        void SetState(Eigen::VectorXd x);
        int NumStates();

        // Serialization
        toml::table Serialize() override;
        void Deserialize(toml::table data) override;

        // Computation
        bool IsReady();

        // Block characteristics
        int GetId();
        std::string GetName();
        bool IsDynamicalSystem();
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

        void RemoveConnectedPort(std::shared_ptr<Port> port);

        // Inputs
        Eigen::VectorXd GetInput(int port_id);

        // Outputs
        void SetOutput(int port_id, Eigen::VectorXd val);

    protected:
        // Block characteristics
        int id_;
        std::string name_;
        bool dynamic_sys_;

        // Diagram membership
        Diagram &diagram_;

        // Timing
        double prev_compute_t_;

        // Internal state
        Eigen::VectorXd x_;
        Eigen::VectorXd dx_;

        // Block ports
        std::vector<std::shared_ptr<Port>> inputs_;
        std::vector<std::shared_ptr<Port>> outputs_;
        std::vector<int> input_ids_;
        std::vector<int> output_ids_;

        // Load ports from TOML
        void LoadPort(toml::table data);
    };
} // namespace ControlBlock
