#include "controlblocks/workspace.h"

void Workspace::Init()
{
    // Set the language
    auto lang = TextEditor::LanguageDefinition::Python();
    editor_.SetLanguageDefinition(lang);
}

void Workspace::Update() { this->Render(); }

void Workspace::Save()
{
    // If the filename is empty then prompt for file dialog.
    if (filename_.empty())
    {
        filename_ = this->SaveFileDialog();
    }
    this->SaveFile();
}

void Workspace::SaveAs()
{
    // Save with a new filename
    filename_ = this->SaveFileDialog();
    this->SaveFile();
}

void Workspace::Load()
{
    // Get the file to open
    std::string path = this->OpenFileDialog();

    // Only load if the filename is non-empty.
    // Note: this may be empty if the user canceled the open operation
    if (!path.empty())
    {
        // Open file
        std::string line, text_to_load = "";
        std::ifstream file(path, std::ifstream::in);
        filename_ = path;

        // Load text
        while (std::getline(file, line))
        {
            text_to_load += line + "\n";
        }

        // Set the text
        editor_.SetText(text_to_load);
    }
}

std::string Workspace::OpenFileDialog()
{
    nfdchar_t *out_path = NULL;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &out_path);

    if (result == NFD_OKAY)
    {
        std::cout << "File: " << out_path << std::endl;
        std::string path(out_path);
        // TODO: check to see if this causes a memory leak.
        return path;
    }
    else if (result == NFD_CANCEL)
    {
    }
    else
    {
        printf("Error: %s\n", NFD_GetError());
    }

    return "";
}

std::string Workspace::SaveFileDialog()
{
    nfdchar_t *out_path = NULL;
    nfdresult_t result = NFD_SaveDialog(NULL, NULL, &out_path);

    if (result == NFD_OKAY)
    {
        std::cout << "File: " << out_path << std::endl;
        std::string path(out_path);
        // TODO: check to see if this causes a memory leak.
        return path;
    }
    else if (result == NFD_CANCEL)
    {
    }
    else
    {
        printf("Error: %s\n", NFD_GetError());
    }

    return "";
}

void Workspace::Render()
{
    auto cpos = editor_.GetCursorPosition();
    ImGui::Begin("Workspace", nullptr,
                 ImGuiWindowFlags_HorizontalScrollbar |
                     ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N", nullptr, true))
            {
                this->NewFile();
            }
            if (ImGui::MenuItem("Open", "Ctrl+O", nullptr, true))
            {
                // Load file from a file dialog
                this->Load();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, true))
            {
                // Save the file
                this->Save();
            }
            if (ImGui::MenuItem("Save As..."))
            {
                // Save the file
                this->SaveAs();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            bool ro = editor_.IsReadOnly();
            if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                editor_.SetReadOnly(ro);
            ImGui::Separator();

            if (ImGui::MenuItem("Undo", "Ctrl+Z", nullptr,
                                !ro && editor_.CanUndo()))
                editor_.Undo();
            if (ImGui::MenuItem("Redo", "Ctrl+Y", nullptr,
                                !ro && editor_.CanRedo()))
                editor_.Redo();

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", "Ctrl+C", nullptr,
                                editor_.HasSelection()))
                editor_.Copy();
            if (ImGui::MenuItem("Cut", "Ctrl+X", nullptr,
                                !ro && editor_.HasSelection()))
                editor_.Cut();
            if (ImGui::MenuItem("Delete", "Del", nullptr,
                                !ro && editor_.HasSelection()))
                editor_.Delete();
            if (ImGui::MenuItem("Paste", "Ctrl+V", nullptr,
                                !ro && ImGui::GetClipboardText() != nullptr))
                editor_.Paste();

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", nullptr, nullptr))
                editor_.SetSelection(
                    TextEditor::Coordinates(),
                    TextEditor::Coordinates(editor_.GetTotalLines(), 0));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Dark Mode"))
                editor_.SetPalette(TextEditor::GetDarkPalette());
            if (ImGui::MenuItem("Light Mode"))
                editor_.SetPalette(TextEditor::GetLightPalette());
            if (ImGui::MenuItem("Retro Blue Mode"))
                editor_.SetPalette(TextEditor::GetRetroBluePalette());
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text(
        "%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1,
        cpos.mColumn + 1, editor_.GetTotalLines(),
        editor_.IsOverwrite() ? "Ovr" : "Ins", editor_.CanUndo() ? "*" : " ",
        editor_.GetLanguageDefinition().mName.c_str(), this->filename_.c_str());

    editor_.Render("TextEditor");
    ImGui::End();

    // Shortcuts
    // New
    if ((ImGui::IsKeyDown(SDL_SCANCODE_LCTRL) ||
         ImGui::IsKeyDown(SDL_SCANCODE_RCTRL)) &&
        ImGui::IsKeyDown(SDL_SCANCODE_N))
    {
        this->NewFile();
    }
    // Open
    if ((ImGui::IsKeyDown(SDL_SCANCODE_LCTRL) ||
         ImGui::IsKeyDown(SDL_SCANCODE_RCTRL)) &&
        ImGui::IsKeyDown(SDL_SCANCODE_O))
    {
        this->Load();
    }
    // Save
    if ((ImGui::IsKeyDown(SDL_SCANCODE_LCTRL) ||
         ImGui::IsKeyDown(SDL_SCANCODE_RCTRL)) &&
        ImGui::IsKeyDown(SDL_SCANCODE_S))
    {
        this->Save();
    }
}

void Workspace::AutoSave()
{
    // Call SaveFile.
    SaveFile();
}

void Workspace::NewFile()
{
    editor_.SetText("");
    filename_ = "";
}

void Workspace::SaveFile()
{
    // Only save if the filename isn't empty.
    // Note: this could still be empty if the user canceled the save operation.
    if (!filename_.empty())
    {
        // Get text to save
        auto textToSave = editor_.GetText();

        // Save text
        std::ofstream file(filename_,
                           std::ofstream::out | std::ofstream::trunc);
        file << textToSave << std::flush;
        file.close();
    }
}
