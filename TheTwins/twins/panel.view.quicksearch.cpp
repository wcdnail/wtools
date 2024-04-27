#include "stdafx.h"
#include "panel.view.quicksearch.h"
#include "panel.view.h"
#include <dh.tracing.h>

namespace Twins
{
    enum 
    {
        CleanTimerId = 2,
        CleaningTimeout = 700,
    };

    QuickSearchHelper::QuickSearchHelper(PanelView& owner)
        : Owner(owner)
        , Accumulator()
        , SkipNext(false)
        , StartIndex(PanelView::InvalidIndex)
    {}

    QuickSearchHelper::~QuickSearchHelper() 
    {}

    void QuickSearchHelper::OnStart()
    {
        int last = StartIndex;
        StartIndex = Owner.iHot;
        if (-1 == last)
            ++StartIndex;
    }

    void QuickSearchHelper::OnAccumulate()
    {
#pragma message(_TODO("Configure case sensitive quick search"))
        bool ignoreCase = true;

        int i = Owner.QuickSearch(StartIndex, Accumulator, ignoreCase);

        if (PanelView::InvalidIndex == i)
            i = Owner.QuickSearch(0, Accumulator, ignoreCase);

        if (PanelView::InvalidIndex != i)
            Owner.SetHot(i);
    }

    void QuickSearchHelper::OnStop()
    {
        StartIndex = PanelView::InvalidIndex;
    }

    void QuickSearchHelper::OnChar(UINT code, UINT rep, UINT flags)
    {
        if (SkipNext)
        {
            SkipNext = false;
            return ;
        }

        if (code > 31)
        {
            if (Accumulator.empty())
                OnStart();

            Accumulator.push_back((wchar_t)code);
            OnAccumulate();
            PrepareCleaning();
        }
    }

    void QuickSearchHelper::PrepareCleaning() const
    {
        Owner.KillTimer(CleanTimerId);
        Owner.SetTimer(CleanTimerId, CleaningTimeout);
    }

    void QuickSearchHelper::Clear()
    {
        Accumulator.clear();
    }

    void QuickSearchHelper::OnTimer(UINT_PTR id)
    {
        if (CleanTimerId == id)
        {
            Clear();
            Owner.KillTimer(id);
            OnStop();
        }
        else
            SetMsgHandled(FALSE);
    }
}
