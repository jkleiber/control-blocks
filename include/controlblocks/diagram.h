
#pragma once

#include <memory>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imnodes.h"
#include "implot.h"

#include "controlblocks/block.h"

// Block types
#include "controlblocks/constant_block.h"

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

private:
    std::vector<std::shared_ptr<ControlBlock::Block>> blocks_;

    int num_items_;

    void Render();

    void AddBlockPopup();
};
