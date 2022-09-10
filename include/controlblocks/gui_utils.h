
#pragma once

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include <iostream>

bool DetectShortcut(SDL_Scancode code1, SDL_Scancode code2);

bool DetectLRShortcut(SDL_Scancode code_left, SDL_Scancode code_right,
                      SDL_Scancode code2);