
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
    Console() {}
    ~Console() {}

    void Render();

    // Console operations
    void Clear();
    void Save();

    // Python redirection
    void Write(std::string str);
    void Flush();

private:
    std::vector<std::string> output_;
};
