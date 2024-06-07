#pragma once

#include "basic.debug.console.h"
#include <atlctrls.h>

namespace DH
{
    class DCListViewImpl: public BasicDebugConsole
    {
    public:
        ~DCListViewImpl() override;
        DCListViewImpl(DebugConsole const& owner);

    private:
        struct StaticInit;

        WTL::CListViewCtrl console_;
        WTL::CReBarCtrl    toolBox_;

        DECLARE_WND_CLASS_EX(_T("WCD_DH_DEBUG_CONSOLE3"), CS_VREDRAW | CS_HREDRAW, (COLOR_WINDOW-1))

        void SetupColumns(CRect const& rc);

        HRESULT PreCreateWindow() override;
        void Save(char const* filePathName) const override;
        HWND CreateConsole() override;
        void PreWrite() override;
        void WriteNarrow(StringItem& siItem) override;
        void WriteWide(StringItem& siItem) override;
        void ClearSelection();
        bool ScrollTo(int nPos, bool bCentered);
        void PostWrite() override;
        void OnDestroy() override;
        void OnCommand(UINT notifyCode, int id, HWND) override;
    };
}
