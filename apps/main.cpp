
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include <iostream>
#include <memory>

#include "controlblocks/gui.h"

#include <pybind11/embed.h>
namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(py_console, module)
{
    py::class_<Console>(module, "stdout")
        .def("write", &Console::Write)
        .def("flush", &Console::Flush);
}

int main(int argc, char **argv)
{
    // Initialize the GUI
    std::shared_ptr<Gui> gui = std::make_shared<Gui>();
    gui->Init();

    // Main loop
    bool done = false;
    while (!done)
    {
        // Update GUI inputs
        try
        {
            gui->Update();
        }
        catch (std::runtime_error e)
        {
            std::cout << e.what() << std::endl;
            done = true;
            break;
        }
        catch (const std::exception &exc)
        {
            // catch anything thrown within try block that derives from
            // std::exception
            std::cerr << exc.what();
        }

        // Render GUI
        gui->Render();
    }

    return 0;
}
