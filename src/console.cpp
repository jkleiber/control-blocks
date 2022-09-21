#include "controlblocks/console.h"

void Console::Render()
{
    ImGui::Begin("Console", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::TextWrapped(
        "This example implements a console with basic coloring, completion "
        "(TAB key) and history (Up/Down keys). A more elaborate "
        "implementation may want to store entries along with extra data such "
        "as timestamp, emitter, etc.");
    ImGui::TextWrapped("Enter 'HELP' for help.");

    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    // static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t =
    // ImGui::GetTime(); AddLog("Spam %f", t); }

    ImGui::Separator();

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &auto_scroll_);
        ImGui::EndPopup();
    }

    // Options, Filter
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve =
        ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve),
                      false, ImGuiWindowFlags_HorizontalScrollbar);

    // Display every line as a separate entry so we can change their color or
    // add custom widgets. If you only want raw text you can use
    // ImGui::TextUnformatted(log.begin(), log.end()); NB- if you have thousands
    // of entries this approach may be too inefficient and may require user-side
    // clipping to only process visible items. The clipper will automatically
    // measure the height of your first item and then "seek" to display only
    // items in the visible area. To use the clipper we can replace your
    // standard loop:
    //      for (int i = 0; i < Items.Size; i++)
    //   With:
    //      ImGuiListClipper clipper;
    //      clipper.Begin(Items.Size);
    //      while (clipper.Step())
    //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
    // - That your items are evenly spaced (same height)
    // - That you have cheap random access to your elements (you can access them
    // given their index,
    //   without processing all the ones before)
    // You cannot this code as-is if a filter is active because it breaks the
    // 'cheap random-access' property. We would need random-access on the
    // post-filtered list. A typical application wanting coarse clipping and
    // filtering may want to pre-compute an array of indices or offsets of items
    // that passed the filtering test, recomputing this array when user changes
    // the filter, and appending newly elements as they are inserted. This is
    // left as a task to the user until we can manage to improve this example
    // code! If your items are of variable height:
    // - Split them into same height items would be simpler and facilitate
    // random-seeking into your list.
    // - Consider using manual call to IsRectVisible() and skipping extraneous
    // decoration from your items.
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        ImVec2(4, 1)); // Tighten spacing
    if (copy_to_clipboard)
        ImGui::LogToClipboard();
    for (int i = 0; i < output_.size(); i++)
    {
        const char *item = output_[i].c_str();
        std::cout << output_[i] << std::flush;

        // Normally you would store more information in your item than just a
        // string. (e.g. make Items[] an array of structure, store color/type
        // etc.)
        ImVec4 color;
        bool has_color = false;
        if (strstr(item, "[error]"))
        {
            color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
            has_color = true;
        }
        else if (strncmp(item, "# ", 2) == 0)
        {
            color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
            has_color = true;
        }
        if (has_color)
            ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(item);
        if (has_color)
            ImGui::PopStyleColor();
    }
    if (copy_to_clipboard)
        ImGui::LogFinish();

    if (scroll_to_bottom_ ||
        (auto_scroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        ImGui::SetScrollHereY(1.0f);
    scroll_to_bottom_ = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    char input_buf[1024];

    // Command-line
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags =
        ImGuiInputTextFlags_EnterReturnsTrue; // |
    // ImGuiInputTextFlags_CallbackCompletion |
    // ImGuiInputTextFlags_CallbackHistory;
    // if (ImGui::InputText("Input", input_buf, IM_ARRAYSIZE(input_buf),
    //                      input_text_flags))
    // {
    //     char *s = input_buf;
    //     // Strtrim(s);
    //     // if (s[0])
    //     //     ExecCommand(s);
    //     strcpy(s, "");
    //     reclaim_focus = true;
    // }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

void Console::Clear() { output_.clear(); }

void Console::Save() {}

void Console::Write(std::string str)
{
    // Write to the console.
    // TODO: limit console size.
    output_.push_back(str);
    std::cout << "log size: " << output_.size() << std::endl;
    std::cout << str << std::flush;
}

void Console::Flush() { std::cout << std::flush; }
