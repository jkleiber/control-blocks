#include "controlblocks/mux_block.h"
#include "controlblocks/diagram.h"

namespace ControlBlock
{

    void MuxBlock::Init(std::string block_name)
    {
        // Port names
        std::string input1_ = block_name + "_in1";
        std::string input2_ = block_name + "_in2";
        output_port_name_ = block_name + "_mux";

        // Input optionality
        std::vector<bool> input_optionals = {true, true};

        // Initialize the block with a single output and no inputs
        std::vector<std::string> input_names = {input1_, input2_};
        std::vector<std::string> output_name = {output_port_name_};

        Block::Init(block_name, input_names, output_name, input_optionals);
    }

    void MuxBlock::Compute()
    {
        // Maintain a vector of Eigen::VectorXds
        std::vector<Eigen::VectorXd> input_vals;

        // Track the size
        int total_size = 0;

        for (size_t i = 0; i < input_ids_.size(); ++i)
        {
            // Get the i-th input
            Eigen::VectorXd val_i = Block::GetInput(input_ids_[i]);

            // Sum up the sizes
            total_size += val_i.size();

            // Save the input
            input_vals.push_back(val_i);
        }

        // Stack the vectors
        Eigen::VectorXd output = ControlUtils::StackVectors(input_vals);

        // Send the output
        Block::SetOutput(output_ids_[0], output);
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
            std::string pin_name = "in" + std::to_string(i);
            ImNodes::BeginInputAttribute(input_ids_[i]);
            ImGui::TextUnformatted(pin_name.c_str());
            ImNodes::EndInputAttribute();
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        // Output group
        ImGui::BeginGroup();
        ImNodes::BeginOutputAttribute(output_ids_[0]);
        const float label_width = ImGui::CalcTextSize(">").x;
        ImGui::Indent(node_width - label_width);
        ImGui::TextUnformatted(">");
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
                // Set the focus to the settings so the window isn't hidden.
                ImGui::SetWindowFocus();
                // Modify number of inputs
                ImGui::InputInt("# inputs", &num_mux_inputs);

                // The mux must have at least one input.
                if (num_mux_inputs < 1)
                {
                    num_mux_inputs = 1;
                }

                // Current size of the input ports pre-modification.
                int original_size = inputs_.size();

                if (num_mux_inputs < original_size)
                {
                    for (int i = 0; i < (original_size - num_mux_inputs); ++i)
                    {
                        // Get the last port ID
                        int id = input_ids_[original_size - 1 - i];

                        // Remove the port from the diagram to free the ImNodes
                        // ID and clear all the connections.
                        diagram_.RemovePort(id, inputs_[original_size - 1 - i]);

                        // Remove the port from this block's lists.
                        input_ids_.pop_back();
                        inputs_.pop_back();
                    }
                }
                else if (num_mux_inputs > original_size)
                {
                    for (int i = 0; i < (num_mux_inputs - original_size); ++i)
                    {
                        // Get new ID from the diagram
                        int new_id = diagram_.AddItem();

                        // Create new input port
                        std::string new_port_name =
                            this->name_ + "_in" +
                            std::to_string(inputs_.size());
                        std::shared_ptr<Port> new_port = std::make_shared<Port>(
                            new_id, new_port_name, PortType::INPUT_PORT,
                            this->id_, true);

                        // Add port info to lists
                        inputs_.push_back(new_port);
                        input_ids_.push_back(new_id);
                    }
                }
            }
            ImGui::End();

            settings_open_ = is_open;
        }
    }

    toml::table MuxBlock::Serialize()
    {
        std::cout << "- Serializing MuxBlock: " << this->name_ << std::endl;

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
            {"type", "MuxBlock"},  {"name", this->name_},   {"id", this->id_},
            {"inputs", input_arr}, {"outputs", output_arr}, {"x_pos", pos.x},
            {"y_pos", pos.y}};

        return tbl;
    }

    void MuxBlock::Deserialize(toml::table data)
    {
        toml::array *in_arr = data["inputs"].as_array();
        if (in_arr != nullptr)
        {
            num_mux_inputs = in_arr->size();
        }
        else
        {
            throw std::runtime_error("No inputs in a mux block on load");
        }

        // Deserialize the general characteristics
        Block::Deserialize(data);
    }

} // namespace ControlBlock
