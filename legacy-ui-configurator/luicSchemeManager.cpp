#include "stdafx.h"
#include "luicSchemeManager.h"
#include "luicRegistry.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>

static constexpr wchar_t REG_ClassicSchemes[] = L"Control Panel\\Appearance\\ClassicSchemes";

CSchemeManager::~CSchemeManager() = default;

CSchemeManager::CSchemeManager()
    : m_Default{L"<<INVALID>>"}
    , m_Schemes{}
{
}

HRESULT CSchemeManager::Initialize()
{
    HRESULT           code{S_OK};
    SchemeMap  tempSchemes{};
    SchemeVec& vNormScheme{tempSchemes[L"Normal"]};

    {
        auto pCurrent = std::make_shared<CScheme>(L"(Current)");
        if (!pCurrent->LoadDefaults()) {
            code = static_cast<HRESULT>(GetLastError());
            return code;
        }
        pCurrent->CopyTo(m_Default);
        vNormScheme.emplace_back(std::move(pCurrent));
    }

    CRegistry regClassics{HKEY_CURRENT_USER, REG_ClassicSchemes};
    if (!regClassics.IsOk()) {
        code = static_cast<HRESULT>(GetLastError());
        return code;
    }
    const int nCount = regClassics.ForEachValue([&tempSchemes](HKEY hKey, PCWSTR szSchemename, int nLen) -> bool {
        CRegistry regScheme{hKey, szSchemename};
        auto pScheme = std::make_shared<CScheme>(StrView{szSchemename, static_cast<size_t>(nLen)});
        if (!pScheme->LoadValues(regScheme)) {
            return false;
        }
        const int nSizeCount = regScheme.ForEachValue([&pScheme, &tempSchemes](HKEY hKey, PCWSTR szSizeName, int nLen) -> bool {
            CRegistry regSizes{hKey, szSizeName};
            String sName{szSizeName, static_cast<size_t>(nLen)};
            if (!pScheme->LoadSizes(sName, regSizes)) {
                return false;
            }
            tempSchemes[sName].push_back(pScheme);
            return true;
        });
        if (CRegistry::ResultFail == nSizeCount) {
            return false;
        }
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
