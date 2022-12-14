#include "controlblocks/wire.h"
#include "controlblocks/diagram.h"

namespace ControlBlock
{
    void Wire::Init() { this->id_ = diagram_.AddItem(); }

    void Wire::Render() { ImNodes::Link(id_, from_id_, to_id_); }

    int Wire::GetId() { return this->id_; }

    int Wire::GetFromId() { return this->from_id_; }

    int Wire::GetToId() { return this->to_id_; }
} // namespace ControlBlock