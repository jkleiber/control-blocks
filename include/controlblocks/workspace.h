
#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include <SDL.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <nfd.h>
#ifdef __cplusplus
}
#endif

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#include "TextEditor.h"

class Workspace
{
public:
    Workspace() {}
    ~Workspace() {}

    void Init();

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

    // File operations
    std::string OpenFileDialog();
    std::string SaveFileDialog();

    /**
     * @brief Render the workspace script
     *
     */
    void Render();

    /**
     * @brief Function to automatically save workspace
     *
     */
    void AutoSave();

    void NewFile();
    void SaveFile();
};