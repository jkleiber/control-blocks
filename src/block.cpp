#include "controlblocks/block.h"
#include "controlblocks/diagram.h"

namespace ControlBlock
{

    void Block::Init(std::string block_name,
                     std::vector<std::string> input_names,
                     std::vector<std::string> output_names)
    {
        id_ = diagram_.AddItem();
        name_ = block_name;

        std::cout << block_name << " created!\n";
        std::cout << "INPUTS:\n";

        // Create input ports
        for (size_t i = 0; i < input_names.size(); ++i)
        {
            int port_id = diagram_.AddItem();
            std::shared_ptr<Port> p = std::make_shared<Port>(
                port_id, input_names[i], PortType::INPUT_PORT, id_);
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

    void Block::Load(toml::table block_tbl)
    {
        // Go through each port and deserialize it to connect to all the other
        // ports
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

    void Block::SetInitial(Eigen::VectorXd x0)
    {
        /**
         * @brief Implement this in a sub-block to set internal block initial
         * conditions.
         */
        // TODO: consider disconnected initial condition blocks to be ready?
    }

    void Block::Compute()
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

        // If the position needs to be updated, do that here.
        if (update_pos_)
        {
            // Create an ImVec2 for positions
            ImVec2 pos(x_pos_, y_pos_);
            this->SetPosition(pos);
            update_pos_ = false;
        }
    }

    void Block::Settings()
    {
        /**
         * @brief Implement this in sub-blocks to provide a settings menu.
         */
    }

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

        toml::table tbl = toml::table{
            {"type", "Block"},     {"name", this->name_},   {"id", this->id_},
            {"inputs", input_arr}, {"outputs", output_arr}, {"x_pos", pos.x},
            {"y_pos", pos.y}};

        return tbl;
    }

    void Block::Deserialize(toml::table data)
    {
        // Get the Block name
        name_ = data["name"].value_or("Block");
        id_ = data["id"].value_or(-1);
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

        // Get the block's position for update
        x_pos_ = data["x_pos"].value_or(0);
        y_pos_ = data["y_pos"].value_or(0);
        update_pos_ = true;
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

    Eigen::VectorXd Block::GetInput(std::string port_name)
    {
        // Collect the input from the correct port.
        // Note: This does not check for duplicate port names. The first port
        // with that name will be chosen. Any duplicates will be ignored.
        for (size_t i = 0; i < inputs_.size(); ++i)
        {
            if (inputs_[i]->GetName() == port_name)
            {
                return inputs_[i]->GetValue();
            }
        }

        throw std::runtime_error("No input port with name: " + port_name +
                                 " for block: " + this->name_);
    }

    void Block::SetOutput(std::string port_name, Eigen::VectorXd val)
    {
        // Output the value to the correct port.
        // Note: This does not check for duplicate port names. The first port
        // with that name will be chosen. Any duplicates will be ignored.
        for (size_t i = 0; i < outputs_.size(); ++i)
        {
            if (outputs_[i]->GetName() == port_name)
            {
                outputs_[i]->SetValue(val);
                break;
            }
        }
    }

    void Block::LoadPort(toml::table tbl)
    {
        // Port ID must exist
        int port_id = tbl["id"].value_or(-1);
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

        // Create the port
        std::shared_ptr<Port> p =
            std::make_shared<Port>(port_id, name, port_type, id_);

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
