
#pragma once

#include "imgui.h"
#include "imnodes.h"

#include "controlblocks/port.h"

class Diagram;

namespace ControlBlock
{
    class Wire
    {

    public:
        Wire(Diagram &d, int from_id, int to_id)
            : diagram_(d), from_id_(from_id), to_id_(to_id)
        {
        }
        ~Wire() {}

        void Init();
        void Render();

        int GetFromId();
        int GetToId();

    private:
        int id_;
        int from_id_;
        int to_id_;

        // Diagram membership
        Diagram &diagram_;
    };
} // namespace ControlBlock