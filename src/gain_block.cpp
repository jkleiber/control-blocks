#include "controlblocks/gain_block.h"

namespace ControlBlock
{

    void GainBlock::Init(std::string block_name)
    {
        // Name the port based on the block
        input_port_name_ = block_name + "_input";
        output_port_name_ = block_name + "_output";

        // Initialize the block with a single output and no inputs
        std::vector<std::string> input_name = {input_port_name_};
        std::vector<std::string> output_name = {output_port_name_};

        Block::Init(block_name, input_name, output_name);

        // Set x0 to a 1x1 zero vector
        x0_ = Eigen::VectorXd::Zero(1);
    }

    void GainBlock::ApplyInitial()
    {
        Block::SetOutput(output_port_name_, x0_);
        Block::Broadcast();
    }

    void GainBlock::SetInitial(Eigen::VectorXd x0) { x0_ = x0; }

    void GainBlock::Compute()
    {
        // Get the input
        Eigen::VectorXd input = Block::GetInput(input_port_name_);

        // Multiply
        Eigen::VectorXd output = val_ * input;

        // Send the output
        Block::SetOutput(output_port_name_, output);
        Block::Broadcast();
    }

    void GainBlock::Render()
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
        ImGui::EndGroup();

        ImGui::SameLine();

        // Output
        ImGui::BeginGroup();
        ImNodes::BeginOutputAttribute(output_ids_[0]);
        ImGui::InputScalar("", ImGuiDataType_Double, &val_, NULL);
        ImNodes::EndOutputAttribute();
        ImGui::EndGroup();

        // Reset item width for the next block.
        ImGui::PopItemWidth();

        ImNodes::EndNode();

        Block::Render();
    }

    toml::table GainBlock::Serialize()
    {
        std::cout << "- Serializing GainBlock: " << this->name_ << std::endl;

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
            {"type", "GainBlock"}, {"name", this->name_},   {"id", this->id_},
            {"inputs", input_arr}, {"outputs", output_arr}, {"x_pos", pos.x},
            {"y_pos", pos.y},      {"gain", val_}};

        return tbl;
    }
} // namespace ControlBlock
