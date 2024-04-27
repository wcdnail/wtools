#pragma once

#include "wcdafx.api.h"
#include "wtl.colorizer.h"
#include "dialogz.types.h"
#include "rect.alloc.h"
#include "string.hp.h"
#include "cf-resources/resource.h"
#include <atltypes.h>
#include <atlwin.h>

namespace Cf
{
    class BasicDialog: ATL::CDialogImpl<BasicDialog>
                     , Colorizer
    {
    public:
        enum { IDD = IDD_BASIC };

        WCDAFX_API BasicDialog(unsigned flags);
        WCDAFX_API BasicDialog(WidecharString const& message, WidecharString const& title, unsigned flags);
        WCDAFX_API virtual ~BasicDialog();
        WCDAFX_API bool Show(HWND parent, Rect const& rc);
        WCDAFX_API void ShowModal(HWND parent);

        DialogResult Result() const { return Rv; }

        WCDAFX_API static unsigned GetCompatFlags(unsigned flags);

    protected:
        typedef ATL::CDialogImpl<BasicDialog> Super;

        friend class Super;
        using Super::SetMsgHandled;

        DialogResult Rv;
        unsigned Flags;
        WidecharString Title;
        WidecharString TextBody;
        CIcon Icon;
        CFont UiFont;
        CFont MsgFont;

        enum ControlIds
        {
          IdFirst = 1001
        , IdOk = IdFirst
        , IdYes
        , IdNo
        , IdCancel
        , IdAbort
        , IdRetry
        , IdIgnore
        , IdClose
        , IdHelp
        , IdContinue
        , IdLast
        };

        CRect SetupIcon(Cf::RectzAllocator<LONG>& rcAlloc);

    private:
        void CreateButtons(Cf::RectzAllocator<LONG>& btnAlloc);
        int OnCreate(LPCREATESTRUCT cs);
        BOOL OnInitDialog(HWND focusedWindow, LPARAM param);
        void OnDestroy();
        void OnKeyDown(UINT key, UINT rep, UINT flags);
        void OnCommand(UINT code, int id, HWND control);

        BEGIN_MSG_MAP_EX(BasicDialog)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_INITDIALOG(OnInitDialog)
            CHAIN_MSG_MAP(Colorizer)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_KEYDOWN(OnKeyDown)
            MSG_WM_COMMAND(OnCommand)
        END_MSG_MAP()
    };
}
