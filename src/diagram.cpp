#include "controlblocks/diagram.h"

void Diagram::Init()
{
    /*
     * Register block types here somehow?
     */
}

void Diagram::Update(GuiData &gui_data)
{
    // Run the simulation if it is active
    if (sim_running_)
    {
        this->Compute();
    }

    // Show the diagram
    auto flags = ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Block Diagram", NULL, flags);

    // Begin the diagram editor
    ImNodes::BeginNodeEditor();

    // Allow blocks to be added to the diagram when the simulation isn't
    // running.
    if (!sim_running_)
    {
        this->AddBlockPopup();
    }

    // Render the blocks in the diagram
    this->Render();

    // End the diagram editor
    ImNodes::EndNodeEditor();
    ImGui::End();

    // Add / Remove new wires to the diagram if the simulation is not active
    if (!sim_running_)
    {
        this->EditWires();
    }
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
    // Find the ports that are connected
    std::shared_ptr<ControlBlock::Port> from_port = GetPortByImNodesId(from);
    ControlBlock::PortType from_type = from_port->GetType();

    std::shared_ptr<ControlBlock::Port> to_port = GetPortByImNodesId(to);
    ControlBlock::PortType to_type = to_port->GetType();

    // Ensure the ports are different types.
    if (from_type != to_type)
    {
        // If the "from" port is an input, then reverse everything so it becomes
        // the output port
        if (from_type == ControlBlock::PortType::INPUT_PORT)
        {
            std::swap(from_port, to_port);
            std::swap(from_type, to_type);
            std::swap(from, to);
        }

        // Ensure the "to" port doesn't have other connections
        if (!to_port->ConnectedInput())
        {
            // Link the ports
            from_port->AddConnection(to_port);
            to_port->AddConnection(from_port);

            std::cout << "Connection added: " << from_port->GetName() << " -> "
                      << to_port->GetName() << std::endl;

            // Create and initialize wire.
            std::shared_ptr<ControlBlock::Wire> wire =
                std::make_shared<ControlBlock::Wire>(*this, from, to);
            wire->Init();

            // Add wire to the diagram
            wires_.push_back(wire);
        }
    }
}

void Diagram::Compute() {}

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
        // Register a new wire
        this->AddWire(start_attr, end_attr);
    }
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

std::shared_ptr<ControlBlock::Port> Diagram::GetPortByImNodesId(int id)
{
    std::shared_ptr<ControlBlock::Port> p;

    // Look through each block and find the port based on ID and type
    for (size_t i = 0; i < blocks_.size(); ++i)
    {
        // See if this block has an input port with this ID
        p = blocks_[i]->FindInputPortByImNodesId(id);
        if (p != nullptr)
        {
            return p;
        }

        // Otherwise check to see if this block has an output port with this ID
        p = blocks_[i]->FindOutputPortByImNodesId(id);
        if (p != nullptr)
        {
            return p;
        }
    }

    // No port found.
    return nullptr;
}