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
        Block::SetOutput(output_ids_[0], output);
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

    toml::table ConstantBlock::Serialize()
    {
        std::cout << "- Serializing ConstantBlock: " << this->name_
                  << std::endl;

        // Get the port serialization for each port
        toml::array output_arr;

        // Outputs
        for (int i = 0; i < outputs_.size(); ++i)
        {
            toml::table port_tbl = outputs_[i]->Serialize();
            output_arr.push_back(port_tbl);
        }

        // Block position
        ImVec2 pos = ImNodes::GetNodeGridSpacePos(this->id_);

        toml::table tbl = toml::table{{"type", "ConstantBlock"},
                                      {"name", this->name_},
                                      {"id", this->id_},
                                      {"outputs", output_arr},
                                      {"x_pos", pos.x},
                                      {"y_pos", pos.y},
                                      {"value", val_}};

        return tbl;
    }

    void ConstantBlock::Deserialize(toml::table tbl)
    {
        // Get the value
        val_ = tbl["value"].value_or(0.0);

        // Deserialize the general components.
        Block::Deserialize(tbl);
    }

} // namespace ControlBlock
