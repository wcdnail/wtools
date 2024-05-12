#include "stdafx.h"
#include "luicUtils.h"

HRESULT IFileDialog_GetDisplayName(IFileDialog& dlgImpl, std::wstring& target)
{
    ATL::CComPtr<IShellItem> pShellItem;
    HRESULT code = dlgImpl.GetResult(&pShellItem);
    if (FAILED(code)) {
        return code;
    }
    PWSTR lpstrName;
    code = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &lpstrName);
    if (FAILED(code)) {
        return code;
    }
    int len = lstrlenW(lpstrName);
    std::wstring{ lpstrName, static_cast<size_t>(len) }.swap(target);
    CoTaskMemFree(lpstrName);
    return S_OK;
}
