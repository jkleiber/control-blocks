
#pragma once

#include <string>

#include "toml++/toml.h"

namespace ControlBlock
{
    class Serializable
    {
    public:
        virtual toml::table Serialize() = 0;
    };
} // namespace ControlBlock
