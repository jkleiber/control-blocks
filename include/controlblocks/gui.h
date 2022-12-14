
#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imnodes.h"
#include "implot.h"
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include "controlblocks/block.h"
#include "controlblocks/diagram.h"
#include "controlblocks/gui_data.h"
#include "controlblocks/workspace.h"

class Gui
{
public:
    Gui(){};

    void Init();
    bool Update();
    void Render();
    void Stop();

    ~Gui() { Stop(); };

private:
    SDL_Window *window;

    SDL_GLContext gl_context;

    Diagram diagram_;
    Workspace workspace_;
    GuiData gui_data_;

    void Menubar();
    void Toolbar();

    void SaveDiagram();
    void LoadDiagram();
};