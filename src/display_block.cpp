#include "controlblocks/display_block.h"

namespace ControlBlock
{

    void DisplayBlock::Init(std::string block_name)
    {
        // Name the port based on the block
        input_port_name_ = block_name + "_input";

        // Initialize the block with a single output and no inputs
        std::vector<std::string> input_name = {input_port_name_};
        std::vector<std::string> output_name;

        Block::Init(block_name, input_name, output_name);
    }

    void DisplayBlock::Compute()
    {
        // Get the input
        val_ = Block::GetInput(input_port_name_);
    }

    void DisplayBlock::Render()
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

        // Input
        ImGui::BeginGroup();
        ImNodes::BeginInputAttribute(input_ids_[0]);
        ImNodes::EndInputAttribute();
        ImGui::SameLine();

        // Print each value in a new line.
        ImGui::BeginGroup();
        for (int i = 0; i < val_.size(); ++i)
        {
            ImGui::Text(std::to_string(val_(i)).c_str());
        }
        ImGui::EndGroup();

        ImGui::EndGroup();

        ImGui::Spacing();

        // Reset item width for the next block.
        ImGui::PopItemWidth();

        ImNodes::EndNode();
    }

} // namespace ControlBlock
