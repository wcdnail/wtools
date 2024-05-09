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

    SchemeVec const& Schemes() const;

private:
    CScheme   m_Default;
    SchemeVec m_Schemes;

    template <typename ReturnType, typename SelfRef>
    static ReturnType& getRefByIndex(SelfRef& thiz, int index);
};

inline CSchemeManager::SchemeVec const& CSchemeManager::Schemes() const { return m_Schemes; }
