#include "controlblocks/constant_block.h"

namespace ControlBlock
{

    void ConstantBlock::Init(std::string block_name)
    {
        // Name the port based on the block
        output_port_name_ = block_name + "_output";

        // Initialize the block with a single output and no inputs
        std::vector<std::string> no_input_names;
        std::vector<std::string> output_name = {output_port_name_};

        Block::Init(block_name, no_input_names, output_name);
    }

    void ConstantBlock::Compute()
    {
        Eigen::VectorXd output = Eigen::VectorXd::Zero(1);
        output(0) = val_;
        Block::SetOutput(output_port_name_, output);
        Block::Broadcast();
    }

    void ConstantBlock::Render()
    {
        ImNodes::BeginNode(this->id_);

        ImGui::Spacing();

        // Ensure the node is just as wide as the title or the minimum width.
        float node_width =
            std::max(min_node_width_, ImGui::CalcTextSize(name_.c_str()).x);
        ImGui::PushItemWidth(node_width);

        ImNodes::BeginNodeTitleBar();

        char name_str[128];
        strcpy(name_str, this->name_.c_str());
        ImGui::InputText("", name_str, IM_ARRAYSIZE(name_str));

        this->name_ = name_str;
        ImNodes::EndNodeTitleBar();

        for (size_t i = 0; i < outputs_.size(); ++i)
        {
            int idx = static_cast<int>(i);

            ImNodes::BeginOutputAttribute(output_ids_[i]);
            ImGui::TextUnformatted(" ");
            ImGui::SameLine();
            ImGui::InputScalar("", ImGuiDataType_Double, &val_, NULL);
            ImNodes::EndOutputAttribute();
        }

        // Reset item width for the next block.
        ImGui::PopItemWidth();

        ImNodes::EndNode();
    }

} // namespace ControlBlock
