
#pragma once

#include <memory>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imnodes.h"
#include "implot.h"

#include "controlblocks/block.h"
#include "controlblocks/port.h"
#include "controlblocks/wire.h"

// Block types
#include "controlblocks/constant_block.h"
#include "controlblocks/display_block.h"
#include "controlblocks/gain_block.h"

typedef struct block_types_t
{
    std::string ui_name;
} BlockType;

/**
 * @brief This class manages all the blocks that are in a control diagram
 *
 */
class Diagram
{

public:
    Diagram() : num_items_(0) {}
    ~Diagram() {}

    void Init();
    void Update();

    int AddItem();

    /**
     * @brief Create and register a new block with the diagram
     *
     * @tparam T The block type to create
     */
    template <typename T> void AddBlock(ImVec2 pos)
    {
        // Create block
        std::shared_ptr<T> T_block = std::make_shared<T>(*this);

        // Initialize block
        T_block->Init();

        // Set the block's position
        T_block->SetPosition(pos);

        // Register block in diagram
        blocks_.push_back(T_block);
    }

    void AddWire(int from, int to);

private:
    std::vector<std::shared_ptr<ControlBlock::Block>> blocks_;
    std::vector<std::shared_ptr<ControlBlock::Wire>> wires_;

    int num_items_;

    // Diagram rendering
    void Render();
    void AddBlockPopup();
    void EditWires();

    // Block searching
    ControlBlock::Port GetPort(int id, ControlBlock::PortType port_type);
};
