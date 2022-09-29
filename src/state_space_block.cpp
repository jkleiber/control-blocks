#include "controlblocks/state_space_block.h"
#include "controlblocks/diagram.h"
#include "controlblocks/python_utils.h"

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

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
        // Load the matrices from the workspace
        std::string errors = "";
        bool is_success = true;

        // A
        if (!PythonUtils::GetWorkspaceVariable<Eigen::MatrixXd>(A_mat_str_,
                                                                &errors, &A_))
        {
            py::print(errors);
            is_success = false;
        }
        // B
        if (!PythonUtils::GetWorkspaceVariable<Eigen::MatrixXd>(B_mat_str_,
                                                                &errors, &B_))
        {
            py::print(errors);
            is_success = false;
        }
        // C
        if (!PythonUtils::GetWorkspaceVariable<Eigen::MatrixXd>(C_mat_str_,
                                                                &errors, &C_))
        {
            py::print(errors);
            is_success = false;
        }
        // D
        if (!PythonUtils::GetWorkspaceVariable<Eigen::MatrixXd>(D_mat_str_,
                                                                &errors, &D_))
        {
            py::print(errors);
            is_success = false;
        }

        // If this is not a success, throw an exception
        if (!is_success)
        {
            throw std::runtime_error("Error: block '" + name_ +
                                     "' missing inputs");
        }

        // Output the initial condition to ensure feedback works
        this->SetOutput(output_ids_[0], x_);
    }

    void StateSpaceBlock::Compute(double t)
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
        const float label_width = ImGui::CalcTextSize("y").x;
        ImGui::Indent(node_width - label_width);
        ImGui::TextUnformatted("y");
        ImNodes::EndOutputAttribute();
        ImGui::EndGroup();

        // Reset item width for the next block.
        ImGui::PopItemWidth();

        ImNodes::EndNode();
    }

    void StateSpaceBlock::Settings()
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
                // Modify matrices
                // A
                char A_str[128];
                strcpy(A_str, A_mat_str_.c_str());
                ImGui::InputText("A:", A_str, IM_ARRAYSIZE(A_str));
                A_mat_str_ = A_str;

                // B
                char B_str[128];
                strcpy(B_str, B_mat_str_.c_str());
                ImGui::InputText("B:", B_str, IM_ARRAYSIZE(B_str));
                B_mat_str_ = B_str;

                // C
                char C_str[128];
                strcpy(C_str, C_mat_str_.c_str());
                ImGui::InputText("C:", C_str, IM_ARRAYSIZE(C_str));
                C_mat_str_ = C_str;

                // D
                char D_str[128];
                strcpy(D_str, D_mat_str_.c_str());
                ImGui::InputText("D:", D_str, IM_ARRAYSIZE(D_str));
                D_mat_str_ = D_str;
            }
            ImGui::End();

            settings_open_ = is_open;
        }
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
                                      {"y_pos", pos.y},
                                      {"A", A_mat_str_},
                                      {"B", B_mat_str_},
                                      {"C", C_mat_str_},
                                      {"D", D_mat_str_}};

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
