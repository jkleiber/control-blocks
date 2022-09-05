#include "controlblocks/block.h"
#include "controlblocks/diagram.h"

namespace ControlBlock
{

    void Block::Init(std::string block_name,
                     std::vector<std::string> input_names,
                     std::vector<std::string> output_names,
                     std::vector<bool> input_optionals, bool dynamic_sys)
    {
        // Block characteristics
        id_ = diagram_.AddItem();
        name_ = block_name;
        dynamic_sys_ = dynamic_sys;

        std::cout << block_name << " created!\n";
        std::cout << "INPUTS:\n";

        // Create input ports
        for (size_t i = 0; i < input_names.size(); ++i)
        {
            bool optional = false;
            if (i < input_optionals.size())
            {
                optional = input_optionals[i];
            }

            int port_id = diagram_.AddItem();
            std::shared_ptr<Port> p = std::make_shared<Port>(
                port_id, input_names[i], PortType::INPUT_PORT, id_, optional);
            inputs_.push_back(p);
            input_ids_.push_back(port_id);

            std::cout << "- " << input_names[i] << std::endl;
        }

        std::cout << "OUTPUTS:\n";
        // Create output ports
        for (size_t i = 0; i < output_names.size(); ++i)
        {
            int port_id = diagram_.AddItem();
            std::shared_ptr<Port> p = std::make_shared<Port>(
                port_id, output_names[i], PortType::OUTPUT_PORT, id_);
            outputs_.push_back(p);
            output_ids_.push_back(port_id);

            std::cout << "- " << output_names[i] << std::endl;
        }
    }

    void Block::Broadcast()
    {
        // Broadcast to the output ports
        for (size_t i = 0; i < outputs_.size(); ++i)
        {
            outputs_[i]->Broadcast();
        }
    }

    void Block::ApplyInitial()
    {
        /**
         * @brief Implement this in a sub-block to have an initialization
         * routine.
         */
    }

    void Block::SetInitial(Eigen::VectorXd x0) { x_ = x0; }

    void Block::Compute(double t)
    {
        /**
         * @brief Implement Compute() in each sub-block to get different I/O
         */
        this->Broadcast();
    }

    void Block::Render()
    {
        /**
         * @brief Implement Render() in each sub-block to get different behavior
         */
    }

    void Block::Settings()
    {
        /**
         * @brief Implement this in sub-blocks to provide a settings menu.
         */
    }

    bool Block::GetDx(Eigen::VectorXd *dx)
    {
        // Don't mess with nullptr, and don't set dx for non-dynamical systems
        if (dx == nullptr || !dynamic_sys_)
        {
            return false;
        }

        // Otherwise, set the pointer to dx_ and return true
        *dx = dx_;
        return true;
    }

    void Block::SetState(Eigen::VectorXd x) { x_ = x; }
    int Block::NumStates() { return x_.size(); }

    toml::table Block::Serialize()
    {
        std::cout << "- Serializing Block: " << this->name_ << std::endl;

        // Get the port serialization for each port
        toml::array input_arr, output_arr;
        for (int i = 0; i < inputs_.size(); ++i)
        {
            toml::table port_tbl = inputs_[i]->Serialize();
            input_arr.push_back(port_tbl);
        }

        // Outputs
        for (int i = 0; i < outputs_.size(); ++i)
        {
            toml::table port_tbl = outputs_[i]->Serialize();
            output_arr.push_back(port_tbl);
        }

        // Block position
        ImVec2 pos = ImNodes::GetNodeGridSpacePos(this->id_);

        toml::table tbl =
            toml::table{{"type", "Block"},       {"name", this->name_},
                        {"id", this->id_},       {"inputs", input_arr},
                        {"outputs", output_arr}, {"x_pos", pos.x},
                        {"y_pos", pos.y},        {"dynamic_sys", dynamic_sys_}};

        return tbl;
    }

    void Block::Deserialize(toml::table data)
    {
        // Get the Block name
        name_ = data["name"].value_or("Block");

        // Determine if the block is a dynamical system, default to false
        dynamic_sys_ = data["dynamic_sys"].value_or(false);

        // Block ID manipulation
        int min_id = data["min_id"].value_or(0);
        id_ = data["id"].value_or(-1) - min_id;
        if (id_ < 0)
        {
            std::runtime_error("No saved ID available for loaded block");
        }

        std::cout << name_ << " loaded!\n";
        std::cout << "INPUTS:\n";

        // Load the ports
        auto input_ports = data["inputs"].as_array();
        auto output_ports = data["outputs"].as_array();

        // If the input ports exist, go through them and create new ports
        if (input_ports != nullptr)
        {
            for (int i = 0; i < input_ports->size(); ++i)
            {
                // Get the port's TOML table and load the port
                toml::table port_tbl = *input_ports->at(i).as_table();
                port_tbl.insert_or_assign("min_id", min_id);
                this->LoadPort(port_tbl);
            }
        }

        std::cout << "OUTPUTS:\n";
        // If the output ports exist, go through them and create new ports
        if (output_ports != nullptr)
        {
            for (int i = 0; i < output_ports->size(); ++i)
            {
                // Get the port's TOML table and load the port
                toml::table port_tbl = *output_ports->at(i).as_table();
                this->LoadPort(port_tbl);
            }
        }

        // Set the block's position
        int x_pos = data["x_pos"].value_or(0);
        int y_pos = data["y_pos"].value_or(0);
        ImVec2 pos(x_pos, y_pos);
        this->SetPosition(pos);
    }

    bool Block::IsReady()
    {
        // If all the input ports are ready, then this block is ready to
        // compute.
        bool ready = true;
        for (size_t i = 0; i < inputs_.size(); ++i)
        {
            ready &= inputs_[i]->IsReady();
        }

        return ready;
    }

    int Block::GetId() { return this->id_; }

    std::string Block::GetName() { return this->name_; }

    bool Block::IsDynamicalSystem() { return this->dynamic_sys_; }

    void Block::SetPosition(ImVec2 pos)
    {
        ImNodes::SetNodeScreenSpacePos(this->id_, pos);
    }

    int Block::NumInputPorts() { return inputs_.size(); }

    std::shared_ptr<Port> Block::GetInputPort(int index)
    {
        if (index < 0 || index > inputs_.size())
        {
            throw std::out_of_range("Input port index " +
                                    std::to_string(index) +
                                    " is out of range for block " +
                                    this->name_ + " when getting port.");
        }
        return inputs_[index];
    }

    int Block::GetInputPortId(int index)
    {
        if (index < 0 || index > inputs_.size())
        {
            throw std::out_of_range("Input port index " +
                                    std::to_string(index) +
                                    " is out of range for block " +
                                    this->name_ + " when getting port ID.");
        }
        return input_ids_[index];
    }

    std::shared_ptr<Port> Block::FindInputPortByImNodesId(int imnode_id)
    {
        // Try to find the imnode id in the input ports
        for (size_t i = 0; i < input_ids_.size(); ++i)
        {
            if (input_ids_[i] == imnode_id)
            {
                return inputs_[i];
            }
        }

        // If nothing was found, return nullptr
        return nullptr;
    }

    int Block::NumOutputPorts() { return outputs_.size(); }

    std::shared_ptr<Port> Block::GetOutputPort(int index)
    {
        if (index < 0 || index > outputs_.size())
        {
            throw std::out_of_range("Output port index " +
                                    std::to_string(index) +
                                    " is out of range for block " +
                                    this->name_ + " when getting port.");
        }
        return outputs_[index];
    }

    int Block::GetOutputPortId(int index)
    {
        if (index < 0 || index > outputs_.size())
        {
            throw std::out_of_range("Output port index " +
                                    std::to_string(index) +
                                    " is out of range for block " +
                                    this->name_ + " when getting port ID.");
        }
        return output_ids_[index];
    }

    std::shared_ptr<Port> Block::FindOutputPortByImNodesId(int imnode_id)
    {
        // Try to find the imnode id in the input ports
        for (size_t i = 0; i < output_ids_.size(); ++i)
        {
            if (output_ids_[i] == imnode_id)
            {
                return outputs_[i];
            }
        }

        // If nothing was found, return nullptr
        return nullptr;
    }

    void Block::RemoveConnectedPort(std::shared_ptr<Port> port_to_remove)
    {
        // If the port to remove is a nullptr, maybe this should be an error,
        // but just show a debug message for now.
        if (port_to_remove == nullptr)
        {
            std::cout << "Port to remove is not valid\n";
            return;
        }

        // Detect if this port is an input port so the port list can be
        // optimized (only need to explore ports of a different type).
        PortType remove_type = port_to_remove->GetType();

        std::vector<std::shared_ptr<Port>> ports_to_check;
        if (remove_type == PortType::INPUT_PORT)
        {
            // Go through all output ports in this block and remove.
            for (int i = 0; i < outputs_.size(); ++i)
            {
                outputs_[i]->RemoveConnection(port_to_remove);
            }
        }
        else
        {
            // Go through all input ports in this block and remove.
            for (int i = 0; i < inputs_.size(); ++i)
            {
                inputs_[i]->RemoveConnection(port_to_remove);
            }
        }
    }

    Eigen::VectorXd Block::GetInput(int port_id)
    {
        // Collect the input from the correct port.
        // Note: This does not check for duplicate port names. The first port
        // with that name will be chosen. Any duplicates will be ignored.
        for (size_t i = 0; i < inputs_.size(); ++i)
        {
            if (inputs_[i]->GetId() == port_id)
            {
                return inputs_[i]->GetValue();
            }
        }

        throw std::runtime_error(
            "No input port with ID: " + std::to_string(port_id) +
            " for block: " + this->name_);
    }

    void Block::SetOutput(int port_id, Eigen::VectorXd val)
    {
        // Output the value to the correct port.
        // Note: This does not check for duplicate port names. The first port
        // with that name will be chosen. Any duplicates will be ignored.
        for (size_t i = 0; i < outputs_.size(); ++i)
        {
            if (outputs_[i]->GetId() == port_id)
            {
                outputs_[i]->SetValue(val);
                break;
            }
        }
    }

    void Block::LoadPort(toml::table tbl)
    {
        // Port ID must exist
        int min_id = tbl["min_id"].value_or(0);
        int port_id = tbl["id"].value_or(-1) - min_id;
        if (port_id < 0)
        {
            throw std::runtime_error("Invalid port ID on load.");
        }

        // PortType must also exist
        int p_type = tbl["type"].value_or(-1);
        if (p_type < 0)
        {
            throw std::runtime_error("Invalid port type on load.");
        }
        PortType port_type = static_cast<PortType>(p_type);

        // Name is optional
        std::string name = tbl["name"].value_or("port");

        // Optionality is optional
        bool is_optional = tbl["is_optional"].value_or(false);

        // Create the port
        std::shared_ptr<Port> p =
            std::make_shared<Port>(port_id, name, port_type, id_, is_optional);

        // Add the port to the list
        if (port_type == PortType::INPUT_PORT)
        {
            inputs_.push_back(p);
            input_ids_.push_back(port_id);
        }
        else
        {
            outputs_.push_back(p);
            output_ids_.push_back(port_id);
        }

        std::cout << "- " << name << std::endl;
    }

} // namespace ControlBlock
