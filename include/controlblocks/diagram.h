
#pragma once

#include <fstream>
#include <memory>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imnodes.h"
#include "implot.h"
#include "toml++/toml.h"

#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include "controlblocks/block.h"
#include "controlblocks/gui_data.h"
#include "controlblocks/port.h"
#include "controlblocks/sim_clock.h"
#include "controlblocks/wire.h"

// Block types
#include "controlblocks/constant_block.h"
#include "controlblocks/display_block.h"
#include "controlblocks/gain_block.h"
#include "controlblocks/mux_block.h"
#include "controlblocks/sum_block.h"

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
    Diagram() : num_items_(0), sim_running_(false), sim_paused_(false) {}
    ~Diagram() {}

    Diagram(const Diagram &diagram)
    {
        this->blocks_ = diagram.blocks_;
        this->wires_ = diagram.wires_;
        this->num_items_ = diagram.num_items_;
        this->sim_running_ = diagram.sim_running_;
    }

    void Init();
    void Update(GuiData &gui_data);

    /**
     * @brief Get the simulation time
     *
     * @return double Simulation time
     */
    double GetTime();

    /**
     * @brief Add an element to the ImGui canvas with a unique ID
     *
     * @return int Unique ID to add to the canvas
     */
    int AddItem();

    /**
     * @brief Update the number of items to be at least as big as a given ID in
     * order to avoid collisions when loading files.
     *
     * @param id ID to set at minimum
     */
    void AddLoadedItem(int id);

    /**
     * @brief Remove an element from the canvas with a given ID. Save that ID
     * for future reassignment.
     * Note: The actual deletion of this element must be handled by the caller.
     *
     * @param id ID to remove
     */
    void RemoveItem(int id);

    /**
     * @brief Remove a port based on its ImGui ID. Calls RemoveItem()
     *
     * @param id ID to remove.
     */
    void RemovePort(int id, std::shared_ptr<ControlBlock::Port> port_to_remove);

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

    template <typename T> void LoadBlock(toml::table block_tbl)
    {
        // Create block
        std::shared_ptr<T> T_block = std::make_shared<T>(*this);

        // Initialize block through de-serialization
        T_block->Deserialize(block_tbl);

        // Ensure the block IDs are considered for future additions.
        this->AddLoadedItem(T_block->GetId());

        // Register block in diagram
        blocks_.push_back(T_block);
    }

    // Wire editing
    void AddWire(int from, int to);
    void RemoveWire(int id);

    // Block removal
    void DetectBlockRemoval();

    // Save / Load / New
    void SaveDiagram(std::string filename);
    void LoadDiagram(std::string filename);
    void ClearDiagram();

private:
    std::vector<std::shared_ptr<ControlBlock::Block>> blocks_;
    std::vector<std::shared_ptr<ControlBlock::Wire>> wires_;

    // ID tracking
    int num_items_;
    std::vector<int> available_ids_;

    // Simulation tracking
    bool sim_running_;
    bool sim_paused_;

    // Simulation timing
    double dt_;
    double tf_;
    SimClock clk_;

    // Diagram simulation
    void InitSim();
    void Compute();

    // Diagram rendering
    void Render();
    void AddBlockPopup();
    void EditWires();
    void EditSettings();

    // Block removal
    void RemoveBlock(int id);

    // Block searching
    std::shared_ptr<ControlBlock::Port> GetPortByImNodesId(int id);
};
