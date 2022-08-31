#include "controlblocks/state_space_block.h"
#include "controlblocks/diagram.h"

namespace ControlBlock
{

    void StateSpaceBlock::Init(std::string block_name)
    {
        std::vector<std::string> input_names = {"u"};
        std::vector<std::string> output_names = {"y"};

        // The u input is optional
        std::vector<bool> input_optionals = {true};

        // Initialize a block that represents a dynamical system
        Block::Init(block_name, input_names, output_names, input_optionals,
                    true);
    }

    void StateSpaceBlock::ApplyInitial()
    {
        // Output the initial condition to ensure feedback works
        this->SetOutput(output_ids_[0], x_);
    }

    void StateSpaceBlock::Compute()
    {
        // Get the input, u
        Eigen::VectorXd u = Block::GetInput(input_ids_[0]);
        if (u.size() == 0)
        {
            u = Eigen::VectorXd::Zero(x_.size());
        }

        // Update the system and get the output using StateSpace
        dx_ = ss.UpdateDynamics(x_, u);
        Eigen::VectorXd y = ss.GetOutput(x_, u);

        // Send the output
        Block::SetOutput(output_ids_[0], y);
        Block::Broadcast();
    }

    void StateSpaceBlock::Render()
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
        ImGui::TextUnformatted("u");
        ImNodes::EndInputAttribute();
        ImGui::EndGroup();

        ImGui::SameLine();

        // Output
        ImGui::BeginGroup();
        ImNodes::BeginOutputAttribute(output_ids_[0]);
        ImGui::TextUnformatted("y");
        ImNodes::EndOutputAttribute();
        ImGui::EndGroup();

        // Reset item width for the next block.
        ImGui::PopItemWidth();

        ImNodes::EndNode();
    }

    toml::table StateSpaceBlock::Serialize()
    {
        std::cout << "- Serializing StateSpaceBlock: " << this->name_
                  << std::endl;

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

        toml::table tbl = toml::table{{"type", "StateSpaceBlock"},
                                      {"name", this->name_},
                                      {"id", this->id_},
                                      {"inputs", input_arr},
                                      {"outputs", output_arr},
                                      {"x_pos", pos.x},
                                      {"y_pos", pos.y}};

        return tbl;
    }

    void StateSpaceBlock::Deserialize(toml::table tbl)
    {
        // Get the matrices
        // TODO

        // Deserialize the general components.
        Block::Deserialize(tbl);
    }

} // namespace ControlBlock
