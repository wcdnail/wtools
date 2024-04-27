#pragma once

#include "brute_cast.h"
#include <atlstr.h>
#include <atltypes.h>
#include <atlctrls.h>

namespace Helpers
{
    template <typename V>
    inline int AddTo(WTL::CComboBox& combo, V value, PCWSTR text)
    {
        int it = combo.AddString(text);

        if (CB_ERR != it)
            combo.SetItemDataPtr(it, (void*)value);

        return it;
    }

    inline void SetDefaultValue(WTL::CComboBox& combo, int n)
    {
        combo.SetCurSel(n);

        CString text;
        combo.GetLBText(n, text);
        combo.SetWindowText(text);
    }

    inline void SetDefaultValue(WTL::CButton& cb, bool val)
    {
        cb.SetCheck(val ? 1 : 0);
    }

    inline bool GetCurrentValue(WTL::CButton const& cb)
    {
        return cb.GetCheck() ? true : false;
    }

    inline WidecharString GetCurrentValueAsString(WTL::CComboBox& combo)
    {
        CStringW text;

        int n = combo.GetCurSel();
        combo.GetLBText(n, text);

        return WidecharString(text.GetString());
    }

    template <typename Return>
    inline Return GetCurrentValueDataAs(WTL::CComboBox const& combo)
    {
        return brute_cast<Return>(combo.GetItemDataPtr(combo.GetCurSel()));
    }
}
