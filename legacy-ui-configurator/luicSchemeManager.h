#pragma once

#include "luicScheme.h"
#include <wcdafx.api.h>
#include <vector>

struct CSchemeManager
{
    DELETE_COPY_MOVE_OF(CSchemeManager);

    using   StrView = CScheme::StrView;
    using    String = CScheme::String;
    using SchemeVec = std::vector<CSchemePtr>;

    ~CSchemeManager();
    CSchemeManager();

    HRESULT Initialize();
    SchemeVec const& GetSchemes() const;

    CSchemePtr const& operator[](int index) const;

private:
    SchemeVec m_Schemes;

    template <typename ReturnType, typename SelfRef>
    static ReturnType& getSchemeRef(SelfRef& thiz, int index);
};

inline CSchemeManager::SchemeVec const& CSchemeManager::GetSchemes() const { return m_Schemes; }
