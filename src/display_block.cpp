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

    void DisplayBlock::Compute(double t)
    {
        // Get the input
        val_ = Block::GetInput(input_ids_[0]);
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

    toml::table DisplayBlock::Serialize()
    {
        std::cout << "- Serializing DisplayBlock: " << this->name_ << std::endl;

        // Get the port serialization for each port
        toml::array input_arr, output_arr;
        for (int i = 0; i < inputs_.size(); ++i)
        {
            toml::table port_tbl = inputs_[i]->Serialize();
            input_arr.push_back(port_tbl);
        }

        // Block position
        ImVec2 pos = ImNodes::GetNodeGridSpacePos(this->id_);

        toml::table tbl = toml::table{
            {"type", "DisplayBlock"}, {"name", this->name_}, {"id", this->id_},
            {"inputs", input_arr},    {"x_pos", pos.x},      {"y_pos", pos.y}};

        return tbl;
    }

} // namespace ControlBlock
