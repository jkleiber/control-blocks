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

    // Render the whole diagram
    this->Render();

    // End the diagram editor
    ImNodes::EndNodeEditor();
    ImGui::End();
}

int Diagram::AddItem()
{
    // Save the current available ID and increment.
    int id = num_items_;
    num_items_ += 1;
    return id;
}

void Diagram::Render()
{
    // Render each block according to its Render() function
    for (size_t i = 0; i < blocks_.size(); ++i)
    {
        blocks_[i]->Render();
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
            std::shared_ptr<ControlBlock::ConstantBlock> const_block =
                std::make_shared<ControlBlock::ConstantBlock>(*this);

            // Initialize
            const_block->Init();

            // Add the new block to the diagram's list of blocks
            blocks_.push_back(const_block);
        }

        ImGui::EndPopup(); // end "Add Block"
    }
}
