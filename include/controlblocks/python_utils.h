
#pragma once

#include <iostream>
#include <map>
#include <string>

#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <pybind11/eval.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace PythonUtils
{
    namespace py = pybind11;

    /**
     * @brief Get a Python variable from the interpreter (global).
     *
     * @tparam T Variable type.
     * @param name Name of the variable to get.
     * @return T variable gotten from the interpreter.
     */
    template <typename T> T GetPythonVariable(const std::string name)
    {
        py::dict global_vars = py::globals();
        if (global_vars.contains(name))
        {
            try
            {
                T val = global_vars[name.c_str()].cast<T>();
                return val;
            }
            catch (const std::exception &e)
            {
                throw std::runtime_error("Error: variable '" + name +
                                         "' cannot be cast to specified type");
            }
        }

        throw std::runtime_error("Error: variable '" + name +
                                 "' does not exist in workspace");
    }

    template <typename T>
    bool GetWorkspaceVariable(const std::string name, std::string *msg, T *var)
    {
        // Fail if the variable is a nullptr
        if (var == nullptr)
        {
            return false;
        }

        bool result = true;
        try
        {
            *var = GetPythonVariable<T>(name);
        }
        catch (const std::exception &e)
        {
            *msg = e.what();
            result = false;
        }

        return result;
    }

} // namespace PythonUtils
