
#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <Eigen/Dense>

namespace ControlBlock
{
    // Forward declare "Block" so the port can know its parent
    class Block;
    typedef enum port_type_t
    {
        INPUT_PORT = 0,
        OUTPUT_PORT
    } PortType;

    class Port
    {
    public:
        Port(std::string name, PortType type, int parent_id)
            : name_(name), type_(type), in_conn_(nullptr), parent_id_(parent_id)
        {
        }

        Port(const Port &p)
        {
            this->name_ = p.name_;
            this->type_ = p.type_;
            this->in_conn_ = p.in_conn_;
            this->out_conns_ = p.out_conns_;
        }

        ~Port() {}

        void operator=(Port &p);
        bool operator==(const Port &a) const;

        // Connection management
        void AddConnection(std::shared_ptr<Port> p);
        void RemoveConnection(std::shared_ptr<Port> p);
        bool ConnectedInput();

        // Inputs
        Eigen::VectorXd GetValue();
        bool IsReady();

        // Outputs
        void Broadcast();
        void Receive(Eigen::VectorXd val, Port &caller);

        // Port characteristics
        std::string GetName();
        PortType GetType();
        void SetValue(Eigen::VectorXd val);
        int GetParentId();

    private:
        std::shared_ptr<Port> in_conn_;
        std::vector<std::shared_ptr<Port>> out_conns_;

        // Port characteristics
        std::string name_;
        uint16_t size_;
        PortType type_;

        // Value
        Eigen::VectorXd val_;

        // If the value is fresh
        bool ready_;

        // Parent block
        int parent_id_;
    };
} // namespace ControlBlock
