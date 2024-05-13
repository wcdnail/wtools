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

bool CBGetCurText(WTL::CComboBox const& ctlCombo, ATL::CString& result)
{
    const int item = ctlCombo.GetCurSel();
    if (CB_ERR == item) {
        return false;
    }
    return CB_ERR != ctlCombo.GetLBText(item, result);
}

bool CBGetCurData(WTL::CComboBox const& ctlCombo, int& result)
{
    const int item = ctlCombo.GetCurSel();
    if (CB_ERR == item) {
        return false;
    }
    const int temp = static_cast<int>(ctlCombo.GetItemData(item));
    if (CB_ERR == temp) {
        return false;
    }
    result = temp;
    return true;
}

bool CBGetCurTextInt(WTL::CComboBox const& ctlCombo, int& result)
{
    ATL::CString strTemp;
    if (!CBGetCurText(ctlCombo, strTemp)) {
        return false;
    }
    int temp = CB_ERR;
    if (_stscanf_s(strTemp.GetString(), _T("%d"), &temp) < 0) {
        return false;
    }
    result = temp;
    return true;
}
