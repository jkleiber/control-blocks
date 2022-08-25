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
            std::shared_ptr<Port> p = std::make_shared<Port>(
                input_names[i], PortType::INPUT_PORT, id_);
            inputs_.push_back(p);
            input_ids_.push_back(diagram_.AddItem());

            std::cout << "- " << input_names[i] << std::endl;
        }

        std::cout << "OUTPUTS:\n";
        // Create output ports
        for (size_t i = 0; i < output_names.size(); ++i)
        {
            std::shared_ptr<Port> p = std::make_shared<Port>(
                output_names[i], PortType::OUTPUT_PORT, id_);
            outputs_.push_back(p);
            output_ids_.push_back(diagram_.AddItem());

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

        const float min_node_width = 100.f;
        ImNodes::BeginNode(this->id_);

        float node_width =
            std::max(min_node_width, ImGui::CalcTextSize(name_.c_str()).x);
        ImGui::PushItemWidth(node_width);

        // Allow the block name to be changed
        ImNodes::BeginNodeTitleBar();
        char name_str[128];
        strcpy(name_str, this->name_.c_str());
        ImGui::InputText("", name_str, IM_ARRAYSIZE(name_str));
        this->name_ = name_str;
        ImNodes::EndNodeTitleBar();

        // Input group
        ImGui::BeginGroup();
        for (size_t i = 0; i < inputs_.size(); ++i)
        {
            ImNodes::BeginInputAttribute(input_ids_[i]);
            std::string in_name = "in" + std::to_string(i);
            ImGui::TextUnformatted(in_name.c_str());
            ImNodes::EndInputAttribute();
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        // Output group
        ImGui::BeginGroup();
        for (size_t i = 0; i < outputs_.size(); ++i)
        {
            int idx = static_cast<int>(i);

            ImNodes::BeginOutputAttribute(output_ids_[i]);
            std::string out_name = "out" + std::to_string(i);
            const float label_width = ImGui::CalcTextSize(out_name.c_str()).x;
            ImGui::Indent(node_width - label_width);
            ImGui::TextUnformatted(out_name.c_str());
            ImNodes::EndOutputAttribute();
        }
        ImGui::EndGroup();

        ImNodes::EndNode();
    }

    void Block::Settings()
    {
        /**
         * @brief Implement this in sub-blocks to provide a settings menu.
         */
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

} // namespace ControlBlock
