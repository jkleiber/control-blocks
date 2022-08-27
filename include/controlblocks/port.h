
#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <Eigen/Dense>
#include <toml++/toml.h>

#include "controlblocks/serializable.h"

namespace ControlBlock
{
    // Forward declare "Block" so the port can know its parent
    class Block;
    typedef enum port_type_t
    {
        INPUT_PORT = 0,
        OUTPUT_PORT
    } PortType;

    class Port : public Serializable
    {
    public:
        Port(int id, std::string name, PortType type, int parent_id,
             bool is_optional = false)
            : id_(id), name_(name), type_(type), in_conn_(nullptr),
              parent_id_(parent_id), is_optional_(is_optional)
        {
            // Default to be scalar 0 in case the port is optional
            val_ = Eigen::VectorXd::Zero(1);
        }

        Port(const Port &p)
        {
            this->id_ = p.id_;
            this->name_ = p.name_;
            this->type_ = p.type_;
            this->in_conn_ = p.in_conn_;
            this->out_conns_ = p.out_conns_;
            this->is_optional_ = p.is_optional_;
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
        int GetId();
        std::string GetName();
        PortType GetType();
        void SetValue(Eigen::VectorXd val);
        int GetParentId();
        bool IsOptional();

        // Serialization
        toml::table Serialize() override;
        void Deserialize(toml::table data) override;

    private:
        // Current ID
        int id_;

        std::shared_ptr<Port> in_conn_;
        std::vector<std::shared_ptr<Port>> out_conns_;

        // Port characteristics
        std::string name_;
        uint16_t size_;
        PortType type_;
        bool is_optional_;

        // Value
        Eigen::VectorXd val_;

        // If the value is fresh
        bool ready_;

        // Parent block
        int parent_id_;

        /**
         * @brief Reset the value of an input port if there is a removed
         * connection.
         *
         */
        void ResetValue();
    };
} // namespace ControlBlock
