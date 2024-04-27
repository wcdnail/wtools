#include "stdafx.h"
#include "ref.var.list.h"
#include <crash.report/exception.h>
#include <boost/throw_exception.hpp>

namespace Ref
{
    List::~List()
    {
    }

    List::List(CharString const& name)
        : list_()
        , name_(name)
    {
    }

    List::List(List& root, CharString const& name)
        : list_()
        , name_(name)
    {
        root.Assign(name, *this);
    }

    List::List(List const& rhs)
        : list_(rhs.list_)
        , name_(rhs.name_)
    {
    }

    List& List::operator = (List const& rhs)
    {
        List temp(rhs);
        Swap(temp);
        return *this;
    }

    void List::Swap(List& rhs)
    {
        list_.swap(rhs.list_);
        name_.swap(rhs.name_);
    }

    Var& List::Get(CharString const& varName)
    {
        CharString listVarName = GetVarName(varName);
        iterator it = list_.find(listVarName);

        if (it == list_.end())
        {
            CharString message = "variable `" + listVarName + "` not found in list";
            throw CrashException(message.c_str(), ERROR_NOT_FOUND);
        }

        return it->second;
    }

    void List::SetName(CharString const& name) 
    { 
        name_ = name; 

        for (iterator it = begin(); it != end(); ++it)
        {
            Var& var = it->second;
            CharString varName = var.GetName();
            CharString::size_type n = varName.rfind('.');
            if (CharString::npos != n)
                varName = varName.substr(++n);

            var.SetName(name_ + "." + varName);
        }
    }

    CharString List::GetVarName(CharString const& varName) const 
    {
        return GetName() + "." + varName; 
    }

    CharString const& List::GetName() const { return name_; }
    List::iterator List::begin() { return list_.begin(); }
    List::iterator List::end() { return list_.end(); }
    List::const_iterator List::begin() const { return list_.begin(); }
    List::const_iterator List::end() const { return list_.end(); }
    List::Accessor List::operator[] (CharString const& varName) { return Accessor(*this, varName); }

    //void List::to_stream(std::ostream& stream) const
    //{
    //    static int depth = -1;

    //    ++depth;
    //    stream << "\n" << std::indent(depth) << "[" << GetName() << "]";
    //    for (const_iterator it = begin(); it != end(); ++it)
    //    {
    //        stream << "\n" << std::indent(depth) << it->first << "=" << it->second;
    //    }
    //    --depth;
    //}
}
