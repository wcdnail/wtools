#include "stdafx.h"
#include "panel.view.selecting.h"
#include "panel.view.status.h"
#include "file.list.h"
#include "dialogz.selunsel.h"
#include <twins.langs/twins.lang.strings.h>
#include <dh.tracing.h>

namespace Twins
{
    enum
    {
        Unmarked = 0,
        Marked,
    };

    SelectHelper::SelectHelper()
        : Selection()
        , RxFiles(true)
        , RxDirs(true)
        , IgnoreCase(true)
    {}

    SelectHelper::~SelectHelper() 
    {}

    SelectHelper::RefStringVec SelectHelper::RxHistory;
    std::wstring SelectHelper::RegExp = L".*";

    bool SelectHelper::Valid() const
    {
        return NULL != Selection.get();
    }

    bool SelectHelper::IsSelected(int i) const
    {
        return Valid() ? (Unmarked != Selection[i]) : false;
    }

    void SelectHelper::Reset(size_t count, ViewStatus& status, HWND owner)
    {
        SelectionSet temp(new Bit[count]);
        Selection.swap(temp);
        Clear(count, status, owner);
    }

    void SelectHelper::Clear(size_t count, ViewStatus& status, HWND owner)
    {
        if (Valid())
        {
            for (size_t i=0; i<count; i++)
                Selection[i] = Unmarked;
        }

        //status.Reset(entries, owner, true);
    }

    void SelectHelper::Toggle(int i)    { if (Valid()) Selection[i] = 1 - Selection[i]; }
    void SelectHelper::Set(int i)       { if (Valid()) Selection[i] = Marked; }
    void SelectHelper::Unset(int i)     { if (Valid()) Selection[i] = Unmarked; }

    void SelectHelper::Toggle(int start, int count)
    {
        for (int i=start; i<count; i++)
            Toggle(i);
    }

    SelectHelper::RefStringVec const& SelectHelper::GetRegexHistory() { return RxHistory; }
    std::wstring const& SelectHelper::GetCurrentRegex() { return RegExp; }
    
    void SelectHelper::StoreMaskToHistory(std::wstring const& mask)
    {
        StringVec::const_iterator it = std::find(RxHistory.cbegin(), RxHistory.cend(), mask);
        if (it == RxHistory.end())
            RxHistory.push_back(mask);

        RegExp = mask;
    }

    static bool SkipSelected(bool selected, bool mark)
    {
        return (mark && selected) || (!mark && !selected);
    }

    void SelectHelper::SelectByRx(Fl::List const& entries, Fl::Entry const& info, int i, std::wstring const& rx, bool mark, ViewStatus& status, bool ignoreCase)
    {
        if (SkipSelected(IsSelected(i), mark) || info.IsUpperDir())
            return ;

        if (info.IsRxMatch(rx, ignoreCase))
        {
            Selection[i] = (mark ? Marked : Unmarked);
            status.Count(entries, info, IsSelected(i));
        }
    }

    std::wstring SelectHelper::GetRegexFromUser(HWND parent, bool mark, bool& selectFiles, bool& selectDirs, bool& ignoreCase)
    {
        SelUnselDialog dlg(mark ? _LS(StrId_Select) : _LS(StrId_Unselect), SelectHelper::GetCurrentRegex().c_str(), RxHistory);

        UINT_PTR dr = dlg.DoModal(parent);
        if (CommonDialog::ResultOk == dr)
        {
            std::wstring rx;
            dlg.GetValues(rx, selectFiles, selectDirs, ignoreCase);
            return rx;
        }

        return L"";
    }

    void SelectHelper::SelectByRegEx(Fl::List const& entries, bool mark, ViewStatus& status, HWND parent /*= NULL*/)
    {
        if (!Valid())
            return ;

        std::wstring rx = GetRegexFromUser(parent, mark, RxFiles, RxDirs, IgnoreCase);
        if (rx.empty())
            return ;

        try
        {
            int counter = 0;
            for (Fl::List::const_iterator it = entries.Begin(); it != entries.End(); ++it)
            {
                bool isFile = !it->IsDir();

                if ((isFile && RxFiles) || (!isFile && RxDirs))
                    SelectByRx(entries, *it, counter, rx, mark, status, IgnoreCase);

                ++counter;
            }

            StoreMaskToHistory(rx);
        }
        catch (std::exception const& ex)
        {
            DH::TPrintf(0, "RegExSel: std::exception - %s\n", ex.what());
        }
    }

    bool SelectHelper::GetSelection(Fl::List const& source, Fl::List& files, Fl::Entry const& hot) const
    {
        int result = 0;
        Fl::List temp;
        for (int i=0; i<source.Count(); i++)
        {
            if (IsSelected(i))
                temp.PushBack(source[i]);
        }

        if ((temp.Count() < 1) && (!hot.IsUpperDir()))
            temp.PushBack(hot);

        files.Swap(temp);
        return files.Count() > 0;
    }

    int SelectHelper::FirstUnselected(int count) const
    {
        if (Valid())
        {
            for (int i=0; i<count; i++)
            {
                if (IsSelected(i))
                    return max(0, i - 1);
            }
        }

        return -1;
    }
}
