
#pragma once

#include <string>
#include <vector>

#include <Eigen/Dense>

typedef enum port_type_t
{
    INPUT = 0,
    OUTPUT
} PortType;

class Port
{
public:
    Port(std::string name, uint16_t size, PortType type)
        : name_(name), size_(size), type_(type)
    {
    }
    ~Port() {}

    void AddConnection(Port &p);

    void SetValue(Eigen::VectorXd val);
    void Broadcast();

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
