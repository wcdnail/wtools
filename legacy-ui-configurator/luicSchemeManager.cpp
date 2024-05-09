#include "stdafx.h"
#include "luicSchemeManager.h"
#include <dh.tracing.h>

CSchemeManager::~CSchemeManager() = default;

CSchemeManager::CSchemeManager()
    : m_Default{L"<<INVALID>>"}
    , m_Schemes{}
{
}

template <typename ReturnType, typename SelfRef>
ReturnType& CSchemeManager::getRefByIndex(SelfRef& thiz, int index)
{
    if (index < 0 || index > static_cast<int>(thiz.m_Schemes.size()) - 1) {
        return thiz.m_Default;
    }
    return thiz.m_Schemes[index] ? *thiz.m_Schemes[index] : thiz.m_Default;
}

CScheme& CSchemeManager::operator[](int index)
{
    return getRefByIndex<CScheme>(*this, index);
}

CScheme const& CSchemeManager::operator[](int index) const
{
    return getRefByIndex<CScheme const>(*this, index);
}

HRESULT CSchemeManager::Initialize()
{
    HRESULT       code{S_OK};
    CSchemePtr pScheme{};

    pScheme = std::make_shared<CScheme>(L"(Current)");
    if (!pScheme->LoadDefaults()) {
        code = static_cast<HRESULT>(GetLastError());
        return code;
    }
    m_Schemes.emplace_back(std::move(pScheme));

    return code;
}
