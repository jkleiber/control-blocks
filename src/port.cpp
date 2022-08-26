#include "controlblocks/port.h"

namespace ControlBlock
{
    void Port::AddConnection(std::shared_ptr<Port> p)
    {
        // Don't allow input ports to have more than one connection
        if (this->type_ == INPUT_PORT && in_conn_ == nullptr)
        {
            in_conn_ = p;
        }
        else if (this->type_ == OUTPUT_PORT)
        {
            // Output ports can have many connections.
            out_conns_.push_back(p);
        }
    }

    void Port::RemoveConnection(std::shared_ptr<Port> p)
    {
        // If this is an input port and the port to remove matches the current
        // connection, then remove it by setting it to nullptr
        if (this->type_ == INPUT_PORT && p == in_conn_)
        {
            in_conn_ = nullptr;
        }
        else if (this->type_ == OUTPUT_PORT)
        {
            // Find and remove the referenced port if it exists
            std::vector<std::shared_ptr<Port>>::iterator loc =
                std::find(out_conns_.begin(), out_conns_.end(), p);
            if (loc != out_conns_.end())
            {
                out_conns_.erase(loc);
            }
        }
    }

    bool Port::ConnectedInput() { return (in_conn_ != nullptr); }

    Eigen::VectorXd Port::GetValue()
    {
        // Since the port value is being read, the port will need to Receive()
        // again before the value is "ready" (fresh).
        ready_ = false;
        return val_;
    }

    bool Port::IsReady() { return ready_; }

    void Port::SetValue(Eigen::VectorXd val)
    {
        // This can only be used with output ports
        if (this->type_ == OUTPUT_PORT)
        {
            val_ = val;
        }
    }

    void Port::Broadcast()
    {
        // Send values to any attached ports
        for (size_t i = 0; i < out_conns_.size(); ++i)
        {
            // If the connection is a nullptr, delete it and skip it.
            // i-- used to ensure the next item isn't skipped.
            if (out_conns_[i] == nullptr)
            {
                out_conns_.erase(out_conns_.begin() + i);
                i--;
                continue;
            }

            // Make the connected ports receive this.
            out_conns_[i]->Receive(val_, *this);
        }
    }

    void Port::Receive(Eigen::VectorXd val, Port &caller)
    {
        // If this is an input port receiving from its subscribed caller,
        // receive the message.
        if (this->type_ == INPUT_PORT && caller == *in_conn_.get())
        {
            val_ = val;
            ready_ = true;
        }
    }

    std::string Port::GetName() { return this->name_; }
    PortType Port::GetType() { return this->type_; }
    int Port::GetParentId() { return this->parent_id_; }
    int Port::GetId() { return this->id_; }

    toml::table Port::Serialize()
    {
        std::cout << "-- Serializing port: " << this->name_ << std::endl;

        // Get the connection ids so there doesn't have to be some deepcopy
        toml::array conns{};

        if (this->type_ == INPUT_PORT)
        {
            // Get the input
            if (in_conn_ != nullptr)
            {
                conns.push_back(this->in_conn_->GetId());
            }
        }
        else
        {
            // Get the output connections
            for (int i = 0; i < out_conns_.size(); ++i)
            {
                conns.push_back(this->out_conns_[i]->GetId());
            }
        }

        int port_type = static_cast<int>(type_);

        // Make the table
        toml::table tbl = toml::table{{"id", this->id_},
                                      {"name", this->name_},
                                      {"parent_id", this->parent_id_},
                                      {"type", port_type},
                                      {"conns", conns}};

        return tbl;
    }

    void Port::Deserialize(toml::table port_tbl)
    {
        // Make the connections
    }

    void Port::operator=(Port &p)
    {
        this->id_ = p.id_;
        this->name_ = p.name_;
        this->type_ = p.type_;
        this->in_conn_ = p.in_conn_;
        this->out_conns_ = p.out_conns_;
    }

    bool Port::operator==(const Port &a) const
    {
        bool is_equal = true;
        is_equal &= this->id_ == a.id_;
        is_equal &= this->name_ == a.name_;
        is_equal &= this->type_ == a.type_;
        is_equal &= this->in_conn_ == a.in_conn_;
        is_equal &= this->out_conns_ == a.out_conns_;

        return is_equal;
    }
} // namespace ControlBlock