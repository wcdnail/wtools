#pragma once

#include "luicScheme.h"
#include <wcdafx.api.h>
#include <filesystem>
#include <vector>

struct CSchemeManager
{
    DELETE_COPY_MOVE_OF(CSchemeManager);

    using   StrView = CScheme::StrView;
    using    String = CScheme::String;
    using SchemeVec = std::vector<CSchemePtr>;
    using      Path = std::filesystem::path;

    ~CSchemeManager();
    CSchemeManager();

    HRESULT Initialize();
    SchemeVec const& GetSchemes() const;

    CSchemePtr const& operator[](int index) const;
    CSchemePtr& operator[](int index);

    int CountWithSameName(String const& name) const;

    int LoadRegistry();
    int LoadWin98THeme(Path const& path);
    int LoadXPRegistry(Path const& path);

    int VanishAllExceptLast();
    int Remove(CScheme const& scheme);

private:
    SchemeVec m_Schemes;

    template <typename ReturnType, typename SelfRef>
    static ReturnType& getSchemeRef(SelfRef& thiz, int index);
};

inline CSchemeManager::SchemeVec const& CSchemeManager::GetSchemes() const { return m_Schemes; }
