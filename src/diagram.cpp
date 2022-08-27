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

        // Remove any blocks set for deletion
        this->DetectBlockRemoval();

        // Allow blocks to have settings edited when the simulation isn't
        // running
        this->EditSettings();
    }
}

double Diagram::GetTime() { return clk_.GetTime(); }

int Diagram::AddItem()
{
    // Default behavior is to use the next available number, counting up from 0.
    int id = num_items_;

    // TODO: This was causing problems with ImNodes drawing somehow?
    // If a diagram gets loaded and then a wire is deleted and you try to add a
    // node, the program crashes due to Line 502 of imnodes.cpp having an assert
    // related to the draw order
    // IM_ASSERT(node_idx_depth_order.Size ==
    // GImNodes->NodeIdxSubmissionOrder.Size);

    // If there are previously allocated IDs that are now available, use the
    // most recently freed ID off the stack.
    // if (available_ids_.size() > 0)
    // {
    //     id = available_ids_[available_ids_.size() - 1];
    //     available_ids_.pop_back();
    //     return id;
    // }

    // Otherwise just keep the default and increment to the next ID.
    num_items_ += 1;
    return id;
}

void Diagram::AddLoadedItem(int id)
{
    // Only update the number of items if the ID is large enough.
    if (id > num_items_)
    {
        num_items_ = id;
    }
}

void Diagram::RemoveItem(int id)
{
    // Add the ID to the list of available IDs for AddItem() to use
    available_ids_.push_back(id);
}

void Diagram::RemovePort(int imnode_id,
                         std::shared_ptr<ControlBlock::Port> port)
{
    // Free the ID for future use.
    RemoveItem(imnode_id);

    // Go through each wire and remove if it is connected to this port ID
    for (int i = 0; i < wires_.size(); ++i)
    {
        // Remove the wires that have connections to this ID.
        if (wires_[i]->GetFromId() == imnode_id ||
            wires_[i]->GetToId() == imnode_id)
        {
            wires_.erase(wires_.begin() + i);
            i--;
        }
    }

    // Go through each block and remove the port connection if it has one.
    for (int i = 0; i < blocks_.size(); ++i)
    {
        blocks_[i]->RemoveConnectedPort(port);
    }
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

void Diagram::RemoveWire(int id)
{
    // Find the wire to delete
    for (int i = 0; i < wires_.size(); ++i)
    {
        if (wires_[i]->GetId() == id)
        {
            // Get the Port IDs
            int from_id = wires_[i]->GetFromId();
            int to_id = wires_[i]->GetToId();

            // Remove the wire from the list
            wires_.erase(wires_.begin() + i);

            // Free the ID
            this->RemoveItem(id);

            // Remove the connection between the ports
            std::shared_ptr<ControlBlock::Port> from_port =
                GetPortByImNodesId(from_id);
            std::shared_ptr<ControlBlock::Port> to_port =
                GetPortByImNodesId(to_id);
            from_port->RemoveConnection(to_port);
            to_port->RemoveConnection(from_port);

            // Done searching
            break;
        }
    }
}

void Diagram::DetectBlockRemoval()
{
    // If any blocks are selected and the user wants to delete them, then delete
    // all selected.
    int num_blocks_selected = ImNodes::NumSelectedNodes();
    if (num_blocks_selected > 0 && ImGui::IsKeyReleased(SDL_SCANCODE_DELETE))
    {
        std::vector<int> selected_block_ids(num_blocks_selected);
        ImNodes::GetSelectedNodes(selected_block_ids.data());

        // Go through each block to delete and delete it
        for (int i = 0; i < num_blocks_selected; ++i)
        {
            this->RemoveBlock(selected_block_ids[i]);
        }
    }
}

void Diagram::RemoveBlock(int id)
{
    // Find the block to remove
    for (size_t i = 0; i < blocks_.size(); ++i)
    {
        if (blocks_[i]->GetId() == id)
        {
            // Disconnect all the input ports
            for (int j = 0; j < blocks_[i]->NumInputPorts(); ++j)
            {
                std::shared_ptr<ControlBlock::Port> port =
                    blocks_[i]->GetInputPort(j);
                this->RemovePort(port->GetId(), port);
            }
            // Disconnect all the output ports
            for (int j = 0; j < blocks_[i]->NumOutputPorts(); ++j)
            {
                std::shared_ptr<ControlBlock::Port> port =
                    blocks_[i]->GetOutputPort(j);
                this->RemovePort(port->GetId(), port);
            }

            // Remove the block from the blocks list
            blocks_.erase(blocks_.begin() + i);

            // Free the ID
            this->RemoveItem(id);

            // Block successfully removed, no more searching required.
            break;
        }
    }
}

void Diagram::SaveDiagram(std::string filename)
{
    // Track the minimum ID
    int min_id = INT_MAX;

    // Go through each block and create a TOML array.
    toml::array blocks_array;
    for (int i = 0; i < blocks_.size(); ++i)
    {
        // Save the ID if it is the minimum
        if (min_id > blocks_[i]->GetId())
        {
            min_id = blocks_[i]->GetId();
        }

        toml::table tbl_i = blocks_[i]->Serialize();
        blocks_array.push_back(tbl_i);
    }

    std::cout << "Diagram serialization done\n";

    toml::table diagram_table =
        toml::table{{"blocks", blocks_array}, {"min_id", min_id}};

    // Output the table to the file.
    std::ofstream file;
    file.open(filename, std::ofstream::out | std::ofstream::trunc);
    file << diagram_table << std::flush;
    file.close();
}

void Diagram::LoadDiagram(std::string filename)
{
    // Read the file
    std::ifstream in_file;
    in_file.open(filename);
    std::stringstream toml_ss;
    toml_ss << in_file.rdbuf();
    std::string toml_str = toml_ss.str();

    // Parse the TOML
    toml::table diagram_tbl = toml::parse(toml_str);

    // Get the minimum ID
    int min_id = diagram_tbl["min_id"].value_or(0);

    // Get the list of blocks
    toml::array *blocks_array = diagram_tbl["blocks"].as_array();
    if (blocks_array != nullptr)
    {
        // Go through and create blocks based on the TOML specs.
        for (int i = 0; i < blocks_array->size(); ++i)
        {
            // Get the block description
            toml::table *block_tbl = blocks_array->at(i).as_table();

            // Only process valid blocks
            if (block_tbl != nullptr)
            {
                // Load a new block based on the block table type
                toml::node *type_node = block_tbl->get("type");
                if (type_node == nullptr)
                {
                    continue;
                }
                std::string block_type =
                    block_tbl->get("type")->value_or("Block");

                // Set the ID based on the minimum ID
                block_tbl->insert_or_assign("min_id", min_id);

                // There's probably a more efficient way to do this using a map
                // of some sort
                // TODO: be efficient
                if (block_type == "ConstantBlock")
                {
                    this->LoadBlock<ControlBlock::ConstantBlock>(*block_tbl);
                }
                else if (block_type == "DisplayBlock")
                {
                    this->LoadBlock<ControlBlock::DisplayBlock>(*block_tbl);
                }
                else if (block_type == "GainBlock")
                {
                    this->LoadBlock<ControlBlock::GainBlock>(*block_tbl);
                }
                else if (block_type == "MuxBlock")
                {
                    this->LoadBlock<ControlBlock::MuxBlock>(*block_tbl);
                }
                else if (block_type == "SumBlock")
                {
                    this->LoadBlock<ControlBlock::SumBlock>(*block_tbl);
                }
            }
        }

        // Activate the ports using AddWire()
        for (int i = 0; i < blocks_array->size(); ++i)
        {
            // Get the block description
            toml::table *block_tbl = blocks_array->at(i).as_table();

            // Only process valid blocks
            if (block_tbl != nullptr)
            {
                // Go through each input port
                toml::node *input_node = block_tbl->get("inputs");
                if (input_node != nullptr)
                {
                    toml::array *input_arr = input_node->as_array();
                    if (input_arr != nullptr)
                    {
                        for (int j = 0; j < input_arr->size(); ++j)
                        {
                            // Find the input port ID and if it doesn't exist
                            // for some reason, ignore it.
                            toml::table *input_port =
                                input_arr->at(j).as_table();
                            if (input_port == nullptr)
                            {
                                continue;
                            }

                            // Try to get the ID, and if it doesn't exist, skip
                            if (input_port->get("id") == nullptr)
                            {
                                continue;
                            }
                            int to_id =
                                input_port->get("id")->value_or(-1) - min_id;
                            if (to_id < 0)
                            {
                                continue;
                            }

                            // Connect the input port to its connection
                            if (input_port->get("conns") == nullptr)
                            {
                                continue;
                            }
                            int from_id =
                                input_port->get("conns")->value_or(-1) - min_id;
                            if (from_id < 0)
                            {
                                continue;
                            }

                            // Add the connection
                            this->AddWire(from_id, to_id);
                            this->AddLoadedItem(from_id);
                            this->AddLoadedItem(to_id);
                        }
                    }
                }

                // Go through each output port
                toml::node *output_node = block_tbl->get("outputs");
                if (output_node != nullptr)
                {
                    toml::array *output_arr = output_node->as_array();
                    if (output_arr != nullptr)
                    {
                        for (int j = 0; j < output_arr->size(); ++j)
                        {
                            // Find the output port ID and if it doesn't exist
                            // for some reason, ignore it.
                            toml::table *output_port =
                                output_arr->at(j).as_table();
                            if (output_port == nullptr)
                            {
                                continue;
                            }

                            // Get the id if it is available.
                            if (output_port->get("id") == nullptr)
                            {
                                continue;
                            }
                            int from_id = output_port->get("id")->value_or(-1);
                            if (from_id < 0)
                            {
                                continue;
                            }

                            // Get all of the output connections
                            if (output_port->get("conns") == nullptr)
                            {
                                continue;
                            }
                            toml::array *to_id_arr =
                                output_port->get("conns")->as_array();
                            if (to_id_arr == nullptr)
                            {
                                // If no connections exist, give up on this
                                // port.
                                continue;
                            }

                            // Go through all connections
                            for (int k = 0; k < to_id_arr->size(); ++k)
                            {
                                int to_id = to_id_arr->at(k).value_or(-1);

                                // If the port to connect doesn't exist, pass on
                                // it
                                if (to_id < 0)
                                {
                                    continue;
                                }

                                // Make the connection
                                this->AddWire(from_id, to_id);
                                this->AddLoadedItem(from_id);
                                this->AddLoadedItem(to_id);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Diagram::ClearDiagram()
{
    // Clear everything
    this->blocks_.clear();
    this->wires_.clear();
    this->available_ids_.clear();
    this->num_items_ = 0;

    // Reset ImNodes
    ImNodes::DestroyContext();
    ImNodes::CreateContext();
    ImNodes::StyleColorsDark();
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
        // std::cout << "All blocks run!\n";

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

    // std::cout << "Time update\n";

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

    // Detect wire deletion
    int link_id;
    if (ImNodes::IsLinkDestroyed(&link_id))
    {
        this->RemoveWire(link_id);
    }

    // Detect multiple wire deletion by user.
    int num_wires_selected = ImNodes::NumSelectedLinks();
    if (num_wires_selected > 0 && ImGui::IsKeyReleased(SDL_SCANCODE_DELETE))
    {
        // Get the selected wires
        std::vector<int> selected_wires(num_wires_selected);
        ImNodes::GetSelectedLinks(selected_wires.data());

        // Remove each wire
        for (const int wire_id : selected_wires)
        {
            this->RemoveWire(wire_id);
        }
    }
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