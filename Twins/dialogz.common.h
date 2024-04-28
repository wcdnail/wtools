#pragma once

#include "tab.bar.h"
#include <wtl.colorizer.h>
#include <atlwin.h>
#include <atlstr.h>
#include "res/resource.h"

namespace Twins
{
    class CommonDialog: public ATL::CDialogImpl<CommonDialog, CF::Colorized::Colorizer>
    {
    public:
        using Super = ATL::CDialogImpl<CommonDialog, CF::Colorized::Colorizer>;

        enum 
        {
            ResultInvalidOperation = -1,
            ResultOk = IDOK,
            ResultCancel = IDCANCEL,
            ResultYes = IDYES,
            ResultNo = IDNO,
            ResultStart = 32,
            ResultYesToAll,
            ResultStartInIdle,
            ResultLast,
        };

        virtual ~CommonDialog();

        void SetCaption(PCWSTR caption);
        PCWSTR GetCaption() const;
        void AddButton(PCWSTR caption, int id = -1, bool defaultId = false, HICON icon = NULL);
        CRect GetRect() const;

    protected:
        friend Super;

        UINT        IDD;
        CString Caption;
        TabBar  Buttons;
        int   DefaultId;

        enum { ID_BUTTONS = 7778 };

        CommonDialog(UINT idd, PCTSTR caption);

        virtual int OnCommandById(int id);
        int GetButtonsCy() const;
        void OnCommand(UINT code, int id, HWND control);
        BOOL OnInitDialog(HWND, LPARAM params);
        void OnDestroy();

        BEGIN_MSG_MAP_EX(CommonDialog)
            if(ProcessColorizerMessage(hWnd, uMsg, wParam, lParam, lResult)) {
                return TRUE;
            }
            MSG_WM_COMMAND(OnCommand)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()
    };

    void LocalizeControl(HWND hwnd, unsigned strId);
    void LocalizeDlgControl(HWND owenr, UINT id, unsigned strId);
}
