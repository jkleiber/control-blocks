
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

class Console
{
public:
    Console() : id_(1), auto_scroll_(true), scroll_to_bottom_(false) {}
    ~Console() {}

    void Render();

    // Console operations
    void Clear();
    void Save();

    // Console properties
    int NumLines();

    // Python redirection
    void Write(std::string str);
    void Flush();

private:
    static std::vector<std::string> output_;

    // Debugging
    int id_;

    // Console properties
    bool auto_scroll_;
    bool scroll_to_bottom_;
};
