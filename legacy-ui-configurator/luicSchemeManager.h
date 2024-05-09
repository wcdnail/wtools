#pragma once

#include "luicScheme.h"
#include <unordered_map>
#include <vector>

struct CSchemeManager
{
    using   StrView = CScheme::StrView;
    using    String = CScheme::String;
    using SchemeVec = std::vector<CSchemePtr>;
    using SchemeMap = std::unordered_map<String, SchemeVec>;

    ~CSchemeManager();
    CSchemeManager();

    HRESULT Initialize();

    SchemeMap const& SchemesMap() const;

private:
    CScheme   m_Default;
    SchemeMap m_Schemes;
};

inline CSchemeManager::SchemeMap const& CSchemeManager::SchemesMap() const { return m_Schemes; }
