#include "stdafx.h"
#include "options.page.h"
#include <dh.tracing.h>

namespace Twins
{
    OptionPage::OptionPage(UINT idd)
        : PendingChanges(false)
        , IDD(idd)
        , TreeRoot(NULL)
        , OnSomethingChanged()
    {}

    OptionPage::~OptionPage()
    {}

    void OptionPage::Init() {}
    void OptionPage::Destroy() {}
    bool OptionPage::ApplyChanges() { return false; }

    BOOL OptionPage::OnInitDialog(HWND, LPARAM param)
    {
        Init();
        return TRUE;
    }

    void OptionPage::OnDestroy()
    {
        Destroy();
        SetMsgHandled(FALSE);
    }

    LRESULT OptionPage::OnNotify(int id, LPNMHDR nh)
    {
        return 0;
    }

    static bool IsModifyNc(int code)
    {
        switch (code)
        {
        case BN_CLICKED:
        case CBN_SELCHANGE:
            return true;
        }

        return false;
    }

    bool OptionPage::HaveChanges() const 
    { 
        return true; 
    }

    void OptionPage::DetectChanges(int code, int id)
    {
        if (IsModifyNc(code))
        {
            PendingChanges = HaveChanges();
            
            if (OnSomethingChanged)
                OnSomethingChanged(*this, IsPendingChanges(), (UINT)id, TreeRoot);
        }
    }

    void OptionPage::OnCommand(UINT code, int id, HWND)
    {
        DetectChanges(code, id);
    }

    void OptionPage::OnApplyChanges()
    {
        PendingChanges = !ApplyChanges();

        if (OnSomethingChanged)
            OnSomethingChanged(*this, PendingChanges, (UINT)-1, TreeRoot);
    }
}
