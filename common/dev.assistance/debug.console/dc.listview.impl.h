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
        int          lastLineCount_;

        DECLARE_WND_CLASS_EX(_T("WCD_DH_DEBUG_CONSOLE3"), CS_VREDRAW | CS_HREDRAW, (COLOR_WINDOW-1))

        void SetupHeader(CRect const& rc);

        HRESULT PreCreateWindow() override;
        void Save(char const* filePathName) const override;
        HWND CreateConsole() override;
        void PreWrite() override;
        void WriteString(char const*) override;
        void WriteString(wchar_t const*) override;
        void PostWrite() override;
        void OnDestroy() override;
        void OnCommand(UINT notifyCode, int id, HWND) override;
    };
}
