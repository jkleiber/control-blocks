
#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <pybind11/eval.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#include "TextEditor.h"

#include "controlblocks/console.h"
#include "controlblocks/file_utils.h"
#include "controlblocks/gui_utils.h"

namespace py = pybind11;

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

    template <typename T> T GetVariable(const std::string name)
    {
        py::dict global_vars = py::globals();
        if (global_vars.contains(name))
        {
            try
            {
                T val = global_vars[name.c_str()].cast<T>();
                return val;
            }
            catch (const std::exception &e)
            {
                throw std::runtime_error("Error: variable '" + name +
                                         "' cannot be cast to specified type");
            }
        }

        throw std::runtime_error("Error: variable '" + name +
                                 "' does not exist in workspace");
    }

private:
    // Editor
    TextEditor editor_;

    // Console
    Console console_;

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