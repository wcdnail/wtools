#pragma once

struct CScopedBoolGuard
{
    ~CScopedBoolGuard();
    CScopedBoolGuard(bool& target);

private:
    bool& target_;
    bool initial_;
};

inline CScopedBoolGuard::~CScopedBoolGuard()
{
    target_ = initial_;
}

inline CScopedBoolGuard::CScopedBoolGuard(bool& target)
    :  target_{target}
    , initial_{target}
{
    target_ = !initial_;
}
