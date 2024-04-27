#pragma once

#include <string>
#include <windef.h>
#include <atlcrack.h>

namespace Twins
{
    class PanelView;

    class QuickSearchHelper
    {
    public:
        QuickSearchHelper(PanelView& owner);
        ~QuickSearchHelper();

        void Skip();

    private:
        friend class PanelView;

        PanelView& Owner;
        std::wstring Accumulator;
        bool SkipNext;
        int StartIndex;

        void OnStart();
        void OnAccumulate();
        void OnStop();

        void OnChar(UINT code, UINT rep, UINT flags);
        void OnTimer(UINT_PTR id);

        void Clear();
        void PrepareCleaning() const;

        BEGIN_MSG_MAP_EX(QuickSearchHelper)
            MSG_WM_CHAR(OnChar)
            MSG_WM_TIMER(OnTimer)
        END_MSG_MAP()
    };

    inline void QuickSearchHelper::Skip() { SkipNext = true; }
}
