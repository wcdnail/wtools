#include "stdafx.h"
#include "panel.view.status.h"
#include "panel.view.selecting.h"
#include "panel.view.scanner.h"
#include "file.list.h"
#include <dh.tracing.h>
#include <twins.langs/twins.lang.strings.h>
#include <string.utils.human.size.h>
#include <generic.increment.h>
#include <limits>

namespace Twins
{
    ViewStatus::ViewStatus()
        : Text()
        , SelFiles(0)
        , SelDirs(0)
        , SelSize(0)
        , MyBackColor(0x202020)
        , MyTextColor(0xdfdfdf)
    {}

    ViewStatus::~ViewStatus() 
    {}

    void ViewStatus::Clear(HWND owner, bool invalidate)
    {
        Text.clear();

        if (invalidate)
            ::InvalidateRect(owner, NULL, FALSE);
    }

    void ViewStatus::Reset(FScanner const& scanner, HWND owner, bool invalidate)
    {
        SelFiles = 0;
        SelDirs = 0;
        SelSize = 0;

        Update(scanner, owner, invalidate);
    }

    void ViewStatus::Update(FScanner const& scanner, HWND owner, bool invalidate)
    {
#pragma message(_TODO("Configure human size units"))

        //WidecharString sizeUnit = _L(StrId_Kb);
        //WidecharString selectedSizeStr = Str::HumanSize(Str::InKils(SelSize));
        //WidecharString totalSizeStr = Str::HumanSize(Str::InKils(entries.SumSize()));
        //
        //wchar_t buffer[256] = {0};
        //::_snwprintf_s(buffer, _countof(buffer)-1, _LS(StrId_Ssfromssfilesdfromddirsdfromd)
        //    , selectedSizeStr.c_str(), sizeUnit.c_str()
        //    , totalSizeStr.c_str(), sizeUnit.c_str()
        //    , SelFiles, entries.FileCount()
        //    , SelDirs, entries.DirCount()
        //    );
        //
        //Text = buffer;
        if (owner && invalidate)
            ::InvalidateRect(owner, NULL, FALSE);
    }

    void ViewStatus::Count(Fl::List const& entries, Fl::Entry const& info, bool add)
    {
        if (!info.IsUpperDir())
        {
            int amount = add ? 1 : -1;

            if (info.IsDir())
                Generic::Increment<int>(SelDirs, amount, 0, (int)entries.DirCount());
            else
                Generic::Increment<int>(SelFiles, amount, 0, (int)entries.FileCount());

            if (add)
                 SelSize += info.GetSize();
            else
                SelSize -= (SelSize < info.GetSize() ? SelSize : info.GetSize());
        }
    }

    void ViewStatus::Count(Fl::List const& entries, SelectHelper const& selector, HWND owner, bool invalidate)
    {
        //int count = 0;
        //for (Fl::List::const_iterator it = entries.Begin(); it != entries.End(); ++it)
        //    Count(entries, *it, selector.IsSelected(count++));
        //
        //Update(entries, owner, invalidate);
    }

    void ViewStatus::OnPaint(CDC& dc, CRect const& rc) const
    {
        dc.FillSolidRect(rc, MyBackColor);

        if (!Text.empty())
        {
            COLORREF oc = dc.SetTextColor(MyTextColor);
            int om = dc.SetBkMode(TRANSPARENT);

            CRect rcText = rc;
            rcText.DeflateRect(2, 2);
            ::DrawTextW(dc, Text.c_str(), (int)Text.length(), rcText, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);

            dc.SetTextColor(oc);
            dc.SetBkMode(om);
        }
    }
}
