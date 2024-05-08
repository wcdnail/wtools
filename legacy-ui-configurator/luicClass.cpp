#include "stdafx.h"
#include "luicClass.h"

Class::~Class() = default;

Class::Class()
{
}

template <typename ReturnType, typename SelfRef>
ReturnType& Class::getRefByIndex(SelfRef& thiz, int index)
{
    switch (index) {
    case CLASS_Invalid: 
    default:
        break;
    }
    static ReturnType dummyVal{};
    return dummyVal;
}

int& Class::operator[](int index)
{
    return getRefByIndex<int>(*this, index);
}

int Class::operator[](int index) const
{
    return getRefByIndex<int const>(*this, index);
}
