#include "controlblocks/diagram.h"

void Diagram::Init()
{
    /*
     * Register block types here somehow?
     */
}

void Diagram::Update(GuiData &gui_data)
{
    // Handle GUI events
    if (gui_data.start && !sim_running_)
    {
        // Reset the simulation if the simulator wasn't previously paused.
        if (!sim_paused_)
        {
            this->InitSim();
        }

        // Set the simulation to running.
        sim_paused_ = false;
        sim_running_ = true;
    }
    if (gui_data.pause && sim_running_)
    {
        // Set the sim to be paused
        sim_paused_ = true;
        sim_running_ = false;
    }
    if (gui_data.stop)
    {
        // Stop the simulator.
        // The next start will re-initialize the diagram.
        sim_running_ = false;
        sim_paused_ = false;
    }

    // Run the simulation if it is active
    if (sim_running_)
    {
        this->Compute();
    }
    else if (!sim_running_ && !sim_paused_)
    {
        // Allow simulation parameters to be changed when the simulation is
        // stopped.
        dt_ = gui_data.dt;
        tf_ = gui_data.sim_time;
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

    // Enable edits outside of the ImNodes context
    if (!sim_running_ && !sim_paused_)
    {
        // Add / Remove new wires to the diagram if the simulation is not active
        this->EditWires();

        // Allow blocks to have settings edited when the simulation isn't
        // running
        this->EditSettings();
    }
}

double Diagram::GetTime() { return clk_.GetTime(); }

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

void Diagram::InitSim()
{
    // Reset the time
    clk_.InitClock(dt_);

    // Initialize all blocks that have an internal state.
    for (std::shared_ptr<ControlBlock::Block> blk : blocks_)
    {
        blk->ApplyInitial();
    }
}

void Diagram::Compute()
{
    // Track if no blocks are ready at all.
    bool no_blocks_ready = false;

    // Track blocks that need to be called
    std::vector<std::shared_ptr<ControlBlock::Block>> blocks_to_call = blocks_;

    // Empty list of blocks already called
    std::vector<std::shared_ptr<ControlBlock::Block>> called_blocks;

    // Run a cycle of computation by giving each block a chance to run.
    while (blocks_to_call.size() > 0 && !no_blocks_ready)
    {
        // // Call Compute() for all blocks that are ready that haven't been
        // called this time cycle.
        int i = 0;
        while (i < blocks_to_call.size())
        {
            // Get the block
            std::shared_ptr<ControlBlock::Block> blk = blocks_to_call[i];
            // std::cout << "Checking " << blk->GetName() << std::endl;

            // Call Compute() if the block is ready.
            if (blk->IsReady())
            {
                // std::cout << "-> Running\n";
                blk->Compute();

                // Move the block to the called list
                called_blocks.push_back(blk);

                // std::cout << "-> Removing from queue\n";
                // Remove this block from the queue.
                std::vector<std::shared_ptr<ControlBlock::Block>>::iterator
                    iter = blocks_to_call.begin() + i;
                blocks_to_call.erase(iter);
            }
            else
            {
                // Increment the index if the block wasn't ready
                ++i;
            }
        }
        std::cout << "All blocks run!\n";

        // Ensure at least one block is ready to run (if not, then all remaining
        // blocks are disconnected)
        no_blocks_ready = true;
        for (size_t i = 0; i < blocks_to_call.size(); ++i)
        {
            if (blocks_to_call[i]->IsReady())
            {
                no_blocks_ready = false;
                break;
            }
        }
    }

    std::cout << "Time update\n";

    // Increment clock for this cycle
    clk_.Increment();

    // Stop the simulation if the time limit is exceeded.
    if (clk_.GetTime() >= tf_)
    {
        sim_paused_ = false;
        sim_running_ = false;
    }
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
    int start_attr, end_attr;
    if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
    {
        // Register a new wire
        this->AddWire(start_attr, end_attr);
    }

    // TODO: detect wire deletion
}

void Diagram::EditSettings()
{
    for (size_t i = 0; i < blocks_.size(); ++i)
    {
        blocks_[i]->Settings();
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
        else if (ImGui::MenuItem("Sum"))
        {
            this->AddBlock<ControlBlock::SumBlock>(click_pos);
        }
        else if (ImGui::MenuItem("Mux"))
        {
            this->AddBlock<ControlBlock::MuxBlock>(click_pos);
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