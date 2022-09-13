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
        // Get program name
        // std::string ws_name = "python";
        wchar_t *program = Py_DecodeLocale(filename_.c_str(), NULL);

        if (program == NULL)
        {
            fprintf(stderr, "Fatal error: cannot decode %s\n",
                    filename_.c_str());
        }

        // Text
        // std::string code_str = editor_.GetText();
        FILE *fp;
        fp = fopen(filename_.c_str(), "r");
        if (fp != NULL)
        {

            Py_SetProgramName(program); /* optional but recommended */
            // PYTHONHOME must be set to C:\Program
            // Files\WindowsApps\PythonSoftwareFoundation.Python.3.10_3.10.2032.0_x64__qbz5n2kfra8p0
            Py_Initialize();
            // PyRun_SimpleString("import numpy as np\n"
            //                    "A = np.identity(2)\n"
            //                    "print('A=', A)\n");

            // PyRun_SimpleString(code_str.c_str());
            // PyObject globals, locals;
            // PyObject *result, *keys;
            // // std::cout << code_str << std::endl;

            // // PyRun_String(code_str.c_str(), 1, &globals, &locals);
            // result = PyRun_File(fp, filename_.c_str(), 0, &globals, &locals);
            // // keys = PyMapping_Keys(&globals);

            // if (result)
            // {
            //     std::cout << "result GOOD\n" << std::flush;
            //     // PyObject_Print(&globals, stdout, 12);
            // }
            // else
            // {
            //     std::cout << "result NULL\n" << std::flush;
            // }

            /**
             * @brief Example code below
             *
             */
            // PyObject *p_name, *p_module, *p_func, *p_val;

            // p_name = PyUnicode_DecodeFSDefault("workspace");
            // if (p_name != NULL)
            // {
            //     p_module = PyImport_Import(p_name);
            //     Py_DECREF(p_name);

            //     if (p_module != NULL)
            //     {
            //         std::cout << "found workspace\n" << std::flush;

            //         // Assuming the function is "run()"
            //         p_func = PyObject_GetAttrString(p_module, "run");

            //         if (p_func && PyCallable_Check(p_func))
            //         {
            //             std::cout << "found run()\n" << std::flush;

            //             // Call the function
            //             p_val = PyObject_CallObject(p_func, NULL);

            //             if (p_val)
            //             {
            //                 std::cout << "Call Success\n" << std::flush;
            //                 PyObject_Print(p_val, stdout, 0);
            //                 Py_DECREF(p_val);
            //             }
            //             else
            //             {
            //                 std::cout << "Call failed\n" << std::flush;
            //                 Py_DECREF(p_func);
            //                 Py_DECREF(p_module);
            //             }
            //         }
            //         else
            //         {
            //             if (PyErr_Occurred())
            //             {
            //                 PyErr_Print();
            //             }
            //         }
            //         Py_XDECREF(p_func);
            //         Py_DECREF(p_module);
            //     }
            //     else
            //     {
            //         PyErr_Print();
            //     }
            // }

            // if (locals != nullptr)
            // {
            //     std::cout << locals->ob_refcnt << std::endl;
            // }
            // else
            // {
            //     std::cout << "Locals nullptr\n";
            // }

            if (Py_FinalizeEx() < 0)
            {
                std::cout << "ERROR\n";
            }

            PyMem_RawFree(program);
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
