#include "stdafx.h"
#include "ref.var.h"
#include <crash.report/exception.h>
#include <sstream>
#include <cerrno>

namespace Ref
{
    Var::Int::~Int() {}

    Var::Int::Int(LString const& name, std::type_info const& type)
        : name_(name)
        , type_(type)
    {
    }

    Var::~Var() {}

    Var::Var(Var const& rhs)
        : ref_(rhs.ref_)
    {}

    void Var::Swap(Var& rhs)
    {
        ref_.swap(rhs.ref_);
    }

    Var& Var::operator = (Var const& rhs)
    {
        Var temp(rhs);
        Swap(temp);
        return *this;
    }

    void Var::SetName(LString const& name) { ref_->SetName(name); }
    LString const& Var::GetName() const { return ref_->GetName(); }
    std::type_info const& Var::GetType() const { return ref_->Type(); }

    void Var::CheckType(LString const& name, std::type_info const& target, std::type_info const& source)
    {
        if (target != source)
        {
            std::stringstream message;
            message << "type mismatch while getting value `" << name.c_str() << "`\n"
                    << "(" << source.name() << " != " << target.name() << ")";

            throw CrashException(message.str().c_str(), ERROR_DATATYPE_MISMATCH);
        }
    }

    template <>
    void Var::Serialize(std::basic_ostream<char>& stream)
    {
    }

    template <>
    void Var::Serialize(std::basic_ostream<wchar_t>& stream)
    {
    }
}
