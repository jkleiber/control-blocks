#include "controlblocks/gui_utils.h"

bool DetectShortcut(SDL_Scancode code1, SDL_Scancode code2)
{
    return (ImGui::IsKeyDown(code1) && ImGui::IsKeyDown(code2));
}

bool DetectLRShortcut(SDL_Scancode code_left, SDL_Scancode code_right,
                      SDL_Scancode code2)
{
    return ((ImGui::IsKeyDown(code_left) || ImGui::IsKeyDown(code_right)) &&
            ImGui::IsKeyDown(code2));
}
