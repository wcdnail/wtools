#pragma once

#include "luicScheme.h"
#include <vector>

struct CSchemeManager
{
    using   StrView = CScheme::StrView;
    using    String = CScheme::String;
    using SchemeVec = std::vector<CSchemePtr>;

    ~CSchemeManager();
    CSchemeManager();

    HRESULT Initialize();
    SchemeVec const& GetSchemes() const;

private:
    CScheme   m_Default;
    SchemeVec m_Schemes;
};

inline CSchemeManager::SchemeVec const& CSchemeManager::GetSchemes() const { return m_Schemes; }
