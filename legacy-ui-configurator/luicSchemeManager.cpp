#include "stdafx.h"
#include "luicSchemeManager.h"
#include "luicRegistry.h"
#include "luicUtils.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>
#include <string.utils.format.h>
#include <functional>
#include <fstream>

static constexpr wchar_t REG_ClassicSchemes[] = L"Control Panel\\Appearance\\ClassicSchemes";

CSchemeManager::~CSchemeManager() = default;

CSchemeManager::CSchemeManager()
    : m_Schemes{}
{
}

template <typename ReturnType, typename SelfRef>
ReturnType& CSchemeManager::getSchemeRef(SelfRef& thiz, int index)
{
    if (index < 0 || index > static_cast<int>(thiz.m_Schemes.size()) - 1) {
        static ReturnType dummy{};
        DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
        return dummy;
    }
    return thiz.m_Schemes[index];
}

CSchemePtr const& CSchemeManager::operator[](int index) const
{
    return getSchemeRef<CSchemePtr const>(*this, index);
}

CSchemePtr& CSchemeManager::operator[](int index)
{
    return getSchemeRef<CSchemePtr>(*this, index);
}

int CSchemeManager::CountWithSameName(String const& name) const
{
    int nCount = 0;
    for (auto const& it: m_Schemes) {
        if (it->Name() == name) {
            ++nCount;
        }
    }
    return nCount;
}

int CSchemeManager::LoadRegistry()
{
    HRESULT                code{S_OK};
    SchemeVec       tempSchemes{};
    const CRegistry regClassics{HKEY_CURRENT_USER, REG_ClassicSchemes};
    if (!regClassics.IsOk()) {
        return IT_Invalid;
    }
    tempSchemes.reserve(m_Schemes.size() + 1);
    const int nCount = regClassics.ForEachValue([this, &tempSchemes](HKEY hKey, PCWSTR szSchemename, int nLen) -> bool {
        const CRegistry regScheme{hKey, szSchemename};
        const String        sName{szSchemename, static_cast<size_t>(nLen)};
        const int          nCount{CountWithSameName(sName)};
        auto pScheme = std::make_shared<CScheme>(sName, nCount);
        if (!pScheme->LoadValues(regScheme)) {
            return false;
        }
        const int nSizeCount = regScheme.ForEachValue([&pScheme, &tempSchemes](HKEY hKey, PCWSTR szSizeName, int nLen) -> bool {
            const CRegistry regSizes{hKey, szSizeName};
            const String       sName{szSizeName, static_cast<size_t>(nLen)};
            if (!pScheme->LoadSizes(sName, regSizes)) {
                return false;
            }
            return true;
        });
        if (CRegistry::ResultFail == nSizeCount) {
            return false;
        }
        tempSchemes.emplace_back(std::move(pScheme));
        return true;
    });
    if (CRegistry::ResultFail == nCount) {
        code = static_cast<HRESULT>(GetLastError());
        const auto codeText = Str::ErrorCode<>::SystemMessage(code);
        DH::TPrintf(L"%s: WARNING: CRegistry::ForEachValue failed: %d '%s'\n", __FUNCTIONW__,
            code, codeText.GetString());
    }
    tempSchemes.append_range(m_Schemes);
    tempSchemes.swap(m_Schemes);
    return 0;
 }

HRESULT CSchemeManager::Initialize()
{
    HRESULT code{S_OK};
    auto pCurrent = std::make_shared<CScheme>(L"(Current)");
    if (!pCurrent->LoadDefaults()) {
        code = static_cast<HRESULT>(GetLastError());
        return code;
    }
    m_Schemes.emplace_back(std::move(pCurrent));
    return code;
}

int CSchemeManager::VanishAllExceptLast()
{
    SchemeVec tempSchemes{};
    tempSchemes.reserve(1);
    tempSchemes.emplace_back(m_Schemes.back());
    tempSchemes.swap(m_Schemes);
    return 0;
}

int CSchemeManager::Remove(CScheme const& scheme)
{
    if (m_Schemes.size() < 2) {
        return IT_Invalid;
    }
    for (auto  it = m_Schemes.begin(); it != m_Schemes.end(); ++it) {
        if (((*it)->Name() == scheme.Name()) && ((*it)->Numba() == scheme.Numba())) {
            m_Schemes.erase(it);
            return 0;
        }
    }
    return IT_Invalid;
}
