#include "controlblocks/console.h"

void Console::Render()
{
    ImGui::Begin("Console", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

    // Display text item by item
    for (size_t i = 0; i < output_.size(); ++i)
    {
        ImGui::TextUnformatted(output_[i].c_str());
    }

    ImGui::End();
}

void Console::Clear() { output_.clear(); }

void Console::Save() {}

void Console::Write(std::string str)
{
    // Write to the console.
    // TODO: limit console size.
    output_.push_back(str);

    std::cout << str << std::flush;
}

void Console::Flush() { std::cout << std::flush; }
