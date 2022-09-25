#include "controlblocks/workspace.h"

namespace py = pybind11;

void Workspace::Init()
{
    // Set the language
    auto lang = TextEditor::LanguageDefinition::Python();
    editor_.SetLanguageDefinition(lang);

    // Initialize the python interpreter
    py::initialize_interpreter();

    // Redirect python to internal console
    py::module::import("py_console");
    py::module::import("sys").attr("stdout") = console_;
}

void Workspace::Stop() { py::finalize_interpreter(); }

void Workspace::Update() { this->Render(); }

void Workspace::Save()
{
    // If the filename is empty then prompt for file dialog.
    bool result = true;
    if (filename_.empty())
    {
        result = SaveFileDialog(&filename_);
    }

    // If the filename isn't empty or has been filled, save the file.
    if (result)
    {
        this->SaveFile();
    }
}

void Workspace::SaveAs()
{
    // Save with a new filename
    if (SaveFileDialog(&filename_))
    {
        this->SaveFile();
    }
}

void Workspace::Load()
{
    // Get the file to open
    std::string path = "";
    OpenFileDialog(&path);

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

void Workspace::Render()
{
    auto cpos = editor_.GetCursorPosition();
    ImGui::Begin("Workspace", nullptr,
                 ImGuiWindowFlags_HorizontalScrollbar |
                     ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

    // Show menubar
    this->MenuBar();

    // Determine if the window has focus
    this->focus_ =
        ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

    ImGui::Text(
        "%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1,
        cpos.mColumn + 1, editor_.GetTotalLines(),
        editor_.IsOverwrite() ? "Ovr" : "Ins", editor_.CanUndo() ? "*" : " ",
        editor_.GetLanguageDefinition().mName.c_str(), this->filename_.c_str());

    editor_.Render("TextEditor");
    ImGui::End();

    // Shortcuts enabled if focused on this window
    if (focus_)
    {
        this->Shortcuts();
    }

    // Also render the console
    console_.Render();
}

void Workspace::MenuBar()
{
    bool is_new = false;
    bool is_open = false;
    bool is_save = false;
    bool is_save_as = false;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N", nullptr, true))
            {
                this->NewFile();
            }
            else if (ImGui::MenuItem("Open", "Ctrl+O", nullptr, true))
            {
                // Load file from a file dialog
                this->Load();
            }
            else if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, true))
            {
                // Save the file
                this->Save();
            }
            else if (ImGui::MenuItem("Save As..."))
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

        if (ImGui::BeginMenu("Run"))
        {
            if (ImGui::MenuItem("Run File", "Ctrl+R"))
            {
                this->RunFile();
            }
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
}

void Workspace::RunFile()
{
    // Save the file
    this->Save();

    if (!filename_.empty())
    {
        // Get the interpreter scope
        py::object scope = py::module_::import("__main__").attr("__dict__");

        // Evaluate a file.
        py::eval_file(filename_, scope);

        // Keep track of the workspace scope somehow here
        // py::exec("print(A)", scope);
        py::dict global_vars = py::globals();

        try
        {
            Eigen::MatrixXd A = this->GetVariable<Eigen::MatrixXd>("A");
            std::cout << A << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}

void Workspace::Shortcuts()
{
    // Do actions from menu selections or shortcuts
    if (DetectLRShortcut(SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL,
                         SDL_SCANCODE_N))
    {
        this->NewFile();
    }
    else if (DetectLRShortcut(SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL,
                              SDL_SCANCODE_O))
    {
        this->Load();
    }
    else if (save_latch_.Get(DetectLRShortcut(
                 SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL, SDL_SCANCODE_S)))
    {
        this->Save();
    }
    else if (run_latch_.Get(DetectLRShortcut(
                 SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL, SDL_SCANCODE_R)))
    {
        this->RunFile();
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
    // Note: this check is necessary because the filename could still be empty
    // if the user canceled the save operation.
    if (!filename_.empty())
    {
        // Get text lines to save
        auto text_lines = editor_.GetTextLines();

        // Save text line by line
        std::ofstream file(filename_,
                           std::ofstream::out | std::ofstream::trunc);
        for (size_t i = 0; i < text_lines.size(); ++i)
        {
            // Add the line to the file
            file << text_lines[i];

            // Only add newlines if the file isn't about to end.
            if (i < (text_lines.size() - 1))
            {
                file << std::endl;
            }
        }

        // Flush the buffer and close the file
        file << std::flush;
        file.close();
    }
}
