#include "controlblocks/mux_block.h"

namespace ControlBlock
{

    void MuxBlock::Init(std::string block_name)
    {
        // Port names
        input1_ = block_name + "_in1";
        input2_ = block_name + "_in2";
        output_port_name_ = block_name + "_mux";

        // Initialize the block with a single output and no inputs
        std::vector<std::string> input_name = {input1_, input2_};
        std::vector<std::string> output_name = {output_port_name_};

        Block::Init(block_name, input_name, output_name);
    }

    void MuxBlock::Compute()
    {
        // Get the input
        Eigen::VectorXd val1 = Block::GetInput(input1_);
        Eigen::VectorXd val2 = Block::GetInput(input2_);

        // Stack the vectors
        Eigen::VectorXd output(val1.size() + val2.size());
        output << val1, val2;

        // Send the output
        Block::SetOutput(output_port_name_, output);
        Block::Broadcast();
    }

    void MuxBlock::Render()
    {
        ImNodes::BeginNode(this->id_);

        float node_width =
            std::max(min_node_width_, ImGui::CalcTextSize(name_.c_str()).x);
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
            ImGui::TextUnformatted("+");
            ImNodes::EndInputAttribute();
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        // Output group
        ImGui::BeginGroup();
        ImNodes::BeginOutputAttribute(output_ids_[0]);
        const float label_width = ImGui::CalcTextSize("Sum").x;
        ImGui::Indent(node_width - label_width);
        ImGui::TextUnformatted("Sum");
        ImNodes::EndOutputAttribute();
        ImGui::EndGroup();

        ImNodes::EndNode();
    }

    void MuxBlock::Settings()
    {
        // If node is double clicked, show the settings
        int hover_id = -1;
        ImNodes::IsNodeHovered(&hover_id);
        if ((hover_id == this->id_ && ImGui::IsMouseDoubleClicked(0)) ||
            settings_open_)
        {
            settings_open_ = true;
            std::string setting_name = this->name_ + " settings";
            bool is_open = true;
            ImGui::Begin(setting_name.c_str(), &is_open);
            if (settings_open_)
            {
                ImGui::InputInt("# inputs", &num_mux_inputs);
            }
            ImGui::End();

            settings_open_ = is_open;
        }
    }

} // namespace ControlBlock
