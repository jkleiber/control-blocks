
#pragma once

#include <string>
#include <vector>

#include <Eigen/Dense>

namespace ControlBlock
{
    typedef enum port_type_t
    {
        INPUT_PORT = 0,
        OUTPUT_PORT
    } PortType;

    class Port
    {
    public:
        Port(std::string name, PortType type) : name_(name), type_(type) {}
        ~Port() {}

        void AddConnection(Port &p);

        // Inputs
        Eigen::VectorXd GetValue();

        // Outputs
        void SetValue(Eigen::VectorXd val);
        void Broadcast();

        // Port characteristics
        std::string GetName();

    private:
        std::vector<Port> conns_;

        // Port characteristics
        std::string name_;
        uint16_t size_;
        PortType type_;

        // Value
        Eigen::VectorXd val_;
    };
} // namespace ControlBlock
