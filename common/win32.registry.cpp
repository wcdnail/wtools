#include "stdafx.h"
#include "win32.registry.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>

CRegistry::~CRegistry() = default;

CRegistry::CRegistry()
    :     m_hKey{nullptr}
    , m_bCreated{false}
{
}

CRegistry::CRegistry(HKEY hRootKey, REGSAM samDesired, StrView path)
    : CRegistry{}
{
    Open(hRootKey, samDesired, path);
}

bool CRegistry::Open(HKEY hRootKey, REGSAM samDesired, StrView path)
{
    bool bCreated{false};
    HRESULT  code{S_OK};
    HKEY   tmpKey{nullptr};
    code = RegOpenKeyExW(hRootKey, path.data(), 0, samDesired, &tmpKey);
    if (ERROR_SUCCESS != code) {
        code = RegCreateKeyExW(hRootKey, path.data(), 0, nullptr, 0, samDesired, nullptr, &tmpKey, nullptr);
        bCreated = true;
    }
    if (!tmpKey) {
        if (ERROR_SUCCESS == code) {
            code = static_cast<HRESULT>(GetLastError());
        }
        const auto codeText = Str::ErrorCode<>::SystemMessage(code);
        DH::TPrintf(TL_Error, L"%s: %s failed: %d '%s'\n", __FUNCTIONW__,
            bCreated ? L"RegCreateKeyExW" : L"RegOpenKeyExW",
            code, codeText.GetString());
        return false;
    }
    m_hKey = tmpKey;
    m_bCreated = bCreated;
    return true;
}

int CRegistry::ForEachValue(ForEachFn const& frRoutine) const
{
    if (!m_hKey) {
        DH::TPrintf(TL_Error, L"%s: !m_hKey\n", __FUNCTIONW__);
        return ResultFail;
    }
    HRESULT  code{ERROR_SUCCESS};
    int    nCount{-1};
    while (ERROR_NO_MORE_ITEMS != code) {
        wchar_t szName[256]{0};
        DWORD     dwNameLen{_countof(szName)};
        ++nCount;
        code = RegEnumKeyExW(m_hKey, nCount, szName, &dwNameLen, nullptr, nullptr, nullptr, nullptr);
        if (ERROR_SUCCESS != code) {
            continue;
        }
        if (!frRoutine(m_hKey, szName, dwNameLen)) {
            return ResultFail;
        }
    }
    return nCount;
}

bool CRegistry::GetValueImpl(StrView name, void* data, size_t dataSize) const
{
    if (!m_hKey) {
        DH::TPrintf(TL_Error, L"%s: !m_hKey\n", __FUNCTIONW__);
        return false;
    }
    DWORD  dwType{0};
    DWORD  dwSize{0};
    HRESULT  code{ERROR_SUCCESS};
    code = RegQueryValueExW(m_hKey, name.data(), nullptr, &dwType, nullptr, &dwSize);
    if (ERROR_SUCCESS != code) {
        const auto codeText = Str::ErrorCode<>::SystemMessage(code);
        DH::TPrintf(TL_Error, L"%s: RegQueryValueExW failed: %d '%s'\n", __FUNCTIONW__,
            code, codeText.GetString());
        return false;
    }
    if (dataSize < dwSize) {
        DH::TPrintf(TL_Error, L"%s: dataSize < dwSize [%d < %d]\n", __FUNCTIONW__, dataSize, dwSize);
        return false;
    }
    code = RegQueryValueExW(m_hKey, name.data(), nullptr, &dwType, static_cast<LPBYTE>(data), &dwSize);
    if (ERROR_SUCCESS != code) {
        const auto codeText = Str::ErrorCode<>::SystemMessage(code);
        DH::TPrintf(TL_Error, L"%s: RegQueryValueExW failed: %d '%s'\n", __FUNCTIONW__,
            code, codeText.GetString());
        return false;
    }
    return true;
}
