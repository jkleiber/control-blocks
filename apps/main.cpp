
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include <iostream>
#include <memory>

#include "controlblocks/code_tools.h"
#include "controlblocks/gui.h"

#include <pybind11/embed.h>
namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(py_console, module)
{
    py::class_<Console>(module, "stdout")
        .def("write", &Console::Write)
        .def("flush", &Console::Flush);
}

static void ConfigurePython()
{
    char *pyhome_path = std::getenv("PYTHONHOME");

    if (pyhome_path != nullptr)
    {
        // WARNING! According to Py_SetPythonHome(..) this must be static, or
        // otherwise ensured that out-lives initialized CPython instance!
        std::string pyhome_str = pyhome_path;
        static std::wstring pyhome_runtime_wstr(pyhome_str.begin(),
                                                pyhome_str.end());
        Py_SetPythonHome(const_cast<wchar_t *>(pyhome_runtime_wstr.data()));
    }
}

int main(int argc, char **argv)
{
    // Configure python using environment variables
    ConfigurePython();

    // Initialize the GUI
    std::shared_ptr<Gui> gui = std::make_shared<Gui>();
    gui->Init();

    // Main loop
    bool done = false;
    while (!done)
    {
        // Update GUI inputs

        CB_CALL(gui->Update());

        // Render GUI
        gui->Render();
    }

    return 0;
}
