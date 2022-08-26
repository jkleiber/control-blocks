
#pragma once

#include <string>

#include "toml++/toml.h"

namespace ControlBlock
{
    class Serializable
    {
    public:
        virtual toml::table Serialize() = 0;
        virtual void Deserialize(toml::table data) = 0;
    };
} // namespace ControlBlock
