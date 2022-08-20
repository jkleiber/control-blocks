#include "controlblocks/port.h"

namespace ControlBlock
{
    void Port::AddConnection(Port &p) { conns_.push_back(p); }

    Eigen::VectorXd Port::GetValue() { return val_; }

    void Port::SetValue(Eigen::VectorXd val) { val_ = val; }

    void Port::Broadcast()
    {
        for (size_t i = 0; i < conns_.size(); ++i)
        {
            conns_[i].SetValue(val_);
        }
    }

    std::string Port::GetName() { return this->name_; }

} // namespace ControlBlock