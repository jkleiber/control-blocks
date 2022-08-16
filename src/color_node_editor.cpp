
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <vector>

#include <SDL_keycode.h>
#include <SDL_timer.h>
#include <imgui.h>
#include <imnodes.h>

#include "controlblocks/color_node_editor.h"

template <class T> T clamp(T x, T a, T b)
{
    return std::min(b, std::max(x, a));
}

void NodeEditorInitialize()
{
    ImNodesIO &io = ImNodes::GetIO();
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
}

void NodeEditorShow() { color_editor.show(); }

void NodeEditorShutdown() {}
