#include "stdafx.h"
#include "luicSchemeManager.h"
#include "luicRegistry.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>

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

HRESULT CSchemeManager::Initialize()
{
    HRESULT           code{S_OK};
    SchemeVec  tempSchemes{};

    {
        auto pCurrent = std::make_shared<CScheme>(L"(Current)");
        if (!pCurrent->LoadDefaults()) {
            code = static_cast<HRESULT>(GetLastError());
            return code;
        }
        tempSchemes.emplace_back(std::move(pCurrent));
    }

    const CRegistry regClassics{HKEY_CURRENT_USER, REG_ClassicSchemes};
    if (!regClassics.IsOk()) {
        code = static_cast<HRESULT>(GetLastError());
        return code;
    }
    const int nCount = regClassics.ForEachValue([&tempSchemes](HKEY hKey, PCWSTR szSchemename, int nLen) -> bool {
        const CRegistry regScheme{hKey, szSchemename};
        auto pScheme = std::make_shared<CScheme>(StrView{szSchemename, static_cast<size_t>(nLen)});
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

    tempSchemes.swap(m_Schemes);
    return code;
}
