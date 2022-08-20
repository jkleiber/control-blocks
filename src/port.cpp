#include "controlblocks/port.h"

void Port::AddConnection(Port &p) { conns_.push_back(p); }

void Port::SetValue(Eigen::VectorXd val) { val_ = val; }

void Port::Broadcast()
{
    for (size_t i = 0; i < conns_.size(); ++i)
    {
        conns_[i].SetValue(val_);
    }
}

std::string Port::GetName() { return this->name_; }