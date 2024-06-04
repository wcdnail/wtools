#pragma once

#include "basic.debug.console.h"

namespace DH
{
    class DCRichEditImpl: public BasicDebugConsole
    {
    public:
        ~DCRichEditImpl() override;
        DCRichEditImpl(DebugConsole const& owner);

    private:
        struct StaticInit;

        WTL::CRichEditCtrl console_;
        WTL::CToolBarCtrl  toolBox_;
        int          lastLineCount_;

        DECLARE_WND_CLASS_EX(_T("WCD_DH_DEBUG_CONSOLE2"), CS_VREDRAW | CS_HREDRAW, (COLOR_WINDOW-1))

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
