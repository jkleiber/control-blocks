#include "controlblocks/block.h"
#include "controlblocks/diagram.h"

namespace ControlBlock
{

    void Block::Init(std::string block_name,
                     std::vector<std::string> input_names,
                     std::vector<uint16_t> input_sizes,
                     std::vector<std::string> output_names,
                     std::vector<uint16_t> output_sizes)
    {
        id_ = diagram_.AddItem();
        name_ = block_name;

        std::cout << block_name << " created!\n";
        std::cout << "INPUTS:\n";

        // Create input ports
        for (size_t i = 0; i < input_names.size(); ++i)
        {
            Port p(input_names[i], input_sizes[i], PortType::INPUT);
            inputs_.push_back(p);
            input_ids_.push_back(diagram_.AddItem());

            std::cout << "- " << input_names[i] << std::endl;
        }

        std::cout << "OUTPUTS:\n";
        // Create output ports
        for (size_t i = 0; i < output_names.size(); ++i)
        {
            Port p(output_names[i], output_sizes[i], PortType::OUTPUT);
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
            const float label_width =
                ImGui::CalcTextSize(inputs_[i].GetName().c_str()).x;
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
