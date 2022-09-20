
#pragma once

#include <fstream>
#include <iostream>
#include <string>

// #include <Python.h>
#include <SDL.h>
#include <pybind11/embed.h>
#include <pybind11/eval.h>
#include <pybind11/pybind11.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#include "TextEditor.h"

#include "controlblocks/file_utils.h"
#include "controlblocks/gui_utils.h"

class Workspace
{
public:
    Workspace() : filename_(""), focus_(false) {}
    ~Workspace() {}

    void Init();
    void Stop();

    // Display
    void Update();

    // File management
    void Save();
    void SaveAs();
    void Load();

    // template <typename T> T GetVariable(std::string name) { return 0; }

private:
    // Editor
    TextEditor editor_;

    // Filename for workspace script
    std::string filename_;

    // Focus
    bool focus_;

    // Latching for shortcuts
    Latch save_latch_;
    Latch run_latch_;

    // Run the active file
    void RunFile();

    /**
     * @brief Render the workspace script
     *
     */
    void Render();

    void MenuBar();
    void Shortcuts();

    /**
     * @brief Function to automatically save workspace
     *
     */
    void AutoSave();

    void NewFile();
    void SaveFile();
};