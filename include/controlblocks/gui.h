
#pragma once

#include <stdexcept>

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

#include "controlblocks/color_node_editor.h"
#include "controlblocks/diagram.h"

class Gui
{
public:
    Gui(){};

    void Init();
    void Update();
    void Render();
    void Stop();

    ~Gui() { Stop(); };

private:
    SDL_Window *window;

    SDL_GLContext gl_context;

    Diagram diagram_;

    void Menubar();
    void Toolbar();
};