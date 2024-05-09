#pragma once

#include "luicScheme.h"
#include <vector>

struct CSchemeManager
{
    using    String = CScheme::String;
    using SchemeVec = std::vector<CSchemePtr>;

    ~CSchemeManager();
    CSchemeManager();

    CScheme& operator[](int index);
    CScheme const& operator[](int index) const;

    HRESULT Initialize();

private:
    CScheme   m_Default;
    SchemeVec m_Schemes;

    template <typename ReturnType, typename SelfRef>
    static ReturnType& getRefByIndex(SelfRef& thiz, int index);
};
