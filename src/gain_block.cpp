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
        ImGui::TextUnformatted("K");
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
    }

} // namespace ControlBlock
