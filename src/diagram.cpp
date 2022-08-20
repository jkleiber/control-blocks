#include "controlblocks/diagram.h"

void Diagram::Init()
{
    /*
     * Register block types here somehow
     */
}

void Diagram::Update()
{
    auto flags = ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Block Diagram", NULL, flags);

    // Begin the diagram editor
    ImNodes::BeginNodeEditor();

    // Allow blocks to be added to the diagram
    this->AddBlockPopup();

    // Render the blocks in the diagram
    this->Render();

    // End the diagram editor
    ImNodes::EndNodeEditor();
    ImGui::End();

    // Add / Remove new wires to the diagram
    this->EditWires();
}

int Diagram::AddItem()
{
    // Save the current available ID and increment.
    int id = num_items_;
    num_items_ += 1;
    return id;
}

void Diagram::AddWire(int from, int to)
{
    // Create and initialize wire.
    std::shared_ptr<ControlBlock::Wire> wire =
        std::make_shared<ControlBlock::Wire>(*this, from, to);
    wire->Init();

    // Add wire to the diagram
    wires_.push_back(wire);
}

void Diagram::Render()
{
    // Render each block according to its Render() function
    for (size_t i = 0; i < blocks_.size(); ++i)
    {
        blocks_[i]->Render();
    }

    // Render each wire
    for (size_t i = 0; i < wires_.size(); ++i)
    {
        wires_[i]->Render();
    }
}

void Diagram::EditWires()
{
    // Detect wire creations
    // TODO: validation
    int start_attr, end_attr;
    if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
    {
        this->AddWire(start_attr, end_attr);
    }
    //     const NodeType start_type = graph_.node(start_attr).type;
    //     const NodeType end_type = graph_.node(end_attr).type;

    //     const bool valid_link = start_type != end_type;
    //     if (valid_link)
    //     {
    //         // Ensure the edge is always directed from the value to
    //         // whatever produces the value
    //         if (start_type != NodeType::value)
    //         {
    //             std::swap(start_attr, end_attr);
    //         }
    //         graph_.insert_edge(start_attr, end_attr);
    //     }
    // }
}

void Diagram::AddBlockPopup()
{
    // Open the block adder popup with a right click
    const bool block_adder_open =
        ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImNodes::IsEditorHovered() &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Right);

    if (block_adder_open && !ImGui::IsAnyItemHovered())
    {
        ImGui::OpenPopup("Add Block");
    }

    // Define the Add Block popup
    if (ImGui::BeginPopup("Add Block"))
    {

        // Get a mouse click location
        const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

        // Loop through available block types
        if (ImGui::MenuItem("Constant"))
        {
            // Create a constant block
            this->AddBlock<ControlBlock::ConstantBlock>(click_pos);
        }
        else if (ImGui::MenuItem("Gain"))
        {
            // Create gain block
            this->AddBlock<ControlBlock::GainBlock>(click_pos);
        }
        else if (ImGui::MenuItem("Display"))
        {
            // Register new display block
            this->AddBlock<ControlBlock::DisplayBlock>(click_pos);
        }

        ImGui::EndPopup(); // end "Add Block"
    }
}

ControlBlock::Port Diagram::GetPort(int id, ControlBlock::PortType port_type)
{
    // Look through each block and find the port based on ID and type
    for (size_t i = 0; i < blocks_.size(); ++i)
    {
        if (port_type == ControlBlock::INPUT_PORT)
        {
        }
        else
        {
        }
    }
}