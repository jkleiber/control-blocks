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
            Port p(input_names[i], PortType::INPUT_PORT);
            inputs_.push_back(p);
            input_ids_.push_back(diagram_.AddItem());

            std::cout << "- " << input_names[i] << std::endl;
        }

        std::cout << "OUTPUTS:\n";
        // Create output ports
        for (size_t i = 0; i < output_names.size(); ++i)
        {
            Port p(output_names[i], PortType::OUTPUT_PORT);
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
            outputs_[i].Broadcast();
        }
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

        const float node_width = 100.f;
        ImNodes::BeginNode(this->id_);

        // Allow the block name to be changed
        ImNodes::BeginNodeTitleBar();
        char name_str[128];
        strcpy(name_str, this->name_.c_str());
        ImGui::InputText("", name_str, IM_ARRAYSIZE(name_str));
        this->name_ = name_str;
        ImNodes::EndNodeTitleBar();

        for (size_t i = 0; i < inputs_.size(); ++i)
        {
            ImNodes::BeginInputAttribute(input_ids_[i]);
            ImGui::TextUnformatted(inputs_[i].GetName().c_str());
            ImNodes::EndInputAttribute();
        }

        ImGui::Spacing();

        for (size_t i = 0; i < outputs_.size(); ++i)
        {
            int idx = static_cast<int>(i);

            ImNodes::BeginOutputAttribute(output_ids_[i]);
            const float label_width =
                ImGui::CalcTextSize(outputs_[i].GetName().c_str()).x;
            ImGui::Indent(node_width - label_width);
            ImGui::TextUnformatted(outputs_[i].GetName().c_str());
            ImNodes::EndOutputAttribute();
        }

        ImNodes::EndNode();
    }

    int Block::GetId() { return this->id_; }

    std::string Block::GetName() { return this->name_; }

    void Block::SetPosition(ImVec2 pos)
    {
        ImNodes::SetNodeScreenSpacePos(this->id_, pos);
    }

    int Block::NumInputPorts() { return inputs_.size(); }

    Port Block::GetInputPort(int index)
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

    int Block::NumOutputPorts() { return outputs_.size(); }

    Port Block::GetOutputPort(int index)
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

    Eigen::VectorXd Block::GetInput(std::string port_name)
    {
        // Collect the input from the correct port.
        // Note: This does not check for duplicate port names. The first port
        // with that name will be chosen. Any duplicates will be ignored.
        for (size_t i = 0; i < inputs_.size(); ++i)
        {
            if (inputs_[i].GetName() == port_name)
            {
                return inputs_[i].GetValue();
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
            if (outputs_[i].GetName() == port_name)
            {
                outputs_[i].SetValue(val);
                break;
            }
        }
    }

} // namespace ControlBlock
