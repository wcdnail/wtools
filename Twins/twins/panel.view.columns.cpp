#include "stdafx.h"
#include "panel.view.columns.h"
#include "panel.view.header.h"
#include "panel.view.sort.h"
#include "iconoz.h"
#include <dh.tracing.h>
#include <string.utils.human.size.h>
#include <twins.langs/twins.lang.strings.h>
#include <color.stuff.h>
#include <algorithm>

namespace Twins
{
    //PanelViewColors::PanelViewColors()
    //    : textColor             (0x707070)
    //    , todayColor            (0xfff0f0)
    //    , yesterdaysColor       (0xe0e0e0)
    //    , thisWeekColor         (0xcf9090)
    //    , prevWeekColor         (0x9f8080)
    //    , hotItemBackgroundColor(0x3c3c3c)
    //    , backgroundColor       (0x050505)
    //    , busyBackgroundColor   (0x000000)
    //    , backgroundBrush       (::CreateSolidBrush(backgroundColor))
    //{
    //    background[0] = backgroundColor;
    //    background[1] = Cf::ReduceColor(background[0], 8, 8, 5);
    //}

    static bool _IsValidId(int id) { return id >= 0 && id < PanelColumns::MaxCount; }
    static void SetHeaderIcon(PanelHeader& header, int id, HICON icon) { header.SetIconById(id, icon); }
    static void SetHeaderIconAsc(PanelHeader& header, int id, bool ascending) { SetHeaderIcon(header, id, ascending ? Icons.HeaderAsc : Icons.HeaderDesc); }
    static void ClearHeaderIcon(PanelHeader& header, int id) { SetHeaderIcon(header, id, NULL); }

    PanelColumns::~PanelColumns()
    {}

    PanelColumns::PanelColumns()
        : iHot(-1)
        , Popup()
    {
        for (int i = 0; i < MaxCount; i++)
        {
            Used[i] = false;
            Ascending[i] = true;
        }

        ResetColumnsToDefault();
    }

    void PanelColumns::ResetColumnsToDefault()
    {
        Used[IdName]        = true; 
        Used[IdExt]         = true; 
        Used[IdSize]        = true; 
        Used[IdDate]        = true;
        Used[IdAttribs]     = false;
        Used[IdShellType]   = false;
    }

    void PanelColumns::ResetWidthsToDefault()
    {
        Widths[IdName]      = 250;
        Widths[IdExt]       = 50;
        Widths[IdSize]      = 100;
        Widths[IdDate]      = 140;
        Widths[IdAttribs]   = 100;
        Widths[IdShellType] = 220;
    }

    int PanelColumns::Widths[MaxCount] = 
    {
      /* Имя          */ 250
    , /* Расширение   */ 50 
    , /* Размер       */ 100
    , /* Дата         */ 140
    , /* Атрибуты     */ 100
    , /* Тип оболочки */ 220
    };

    unsigned PanelColumns::NameId[MaxCount] =
    {
      StrId_Name
    , StrId_Type
    , StrId_Size
    , StrId_Date
    , StrId_Attributes
    , StrId_Shelltype
    };

    inline void PanelColumns::AddColumn(PanelHeader& header, int id)
    {
        header.Add(id, _LS(NameId[id]), Widths[id]);
    }

    void PanelColumns::ResetHeader(PanelHeader& header) const
    {
        header.Clear();

        for (int i = 0; i < MaxCount; i++)
            if (Used[i])
                AddColumn(header, i);

        header.SetSelectedById(iHot);
        SetHeaderIconAsc(header, iHot, Ascending[iHot]);
    }

    void PanelColumns::OnCreate(PanelHeader& header)
    {
        AssignedHeaders.push_back(&header);
        header.OnColumnResizeDone() = std::bind(&PanelColumns::OnResizeDone, this, std::placeholders::_1, std::placeholders::_2);
        ResetHeader(header);
    }

    PanelColumns::Headers PanelColumns::AssignedHeaders;

    void PanelColumns::OnDestroy(PanelHeader& header)
    {
        Headers::iterator i = std::find(AssignedHeaders.begin(), AssignedHeaders.end(), &header);
        if (i != AssignedHeaders.end())
            AssignedHeaders.erase(i);
    }

    void PanelColumns::OnResizeDone(int id, int nw)
    {
        if (_IsValidId(id))
        {
            Widths[id] = nw;

            for (Headers::iterator i = AssignedHeaders.begin(); i != AssignedHeaders.end(); ++i)
                (*i)->SetColumnWidthById(id, nw, true);
        }
    }

    enum 
    {
        PopupIdNone = -1,
        PopupIdClearSort = -2,
        PopupIdBase = 5,
    };

    void PanelColumns::RebuildPopup(int cmdbase)
    {
        CMenu temp;
        if (temp.CreatePopupMenu())
        {
            temp.AppendMenu(MF_STRING | MF_DISABLED, PopupIdNone, _LS(StrId_Choosecolumns));
            temp.AppendMenu(MF_STRING, PopupIdClearSort, _LS(StrId_Withoutsort));
            temp.AppendMenu(MF_SEPARATOR);

            for (int i=IdName; i<MaxCount; i++)
                temp.AppendMenu((Used[i] ? MF_CHECKED : 0) | MF_STRING, cmdbase + i, _LS(NameId[i]));

            temp.AppendMenu(MF_SEPARATOR);
            temp.AppendMenu(MF_STRING, cmdbase + MaxCount, _LS(StrId_Bydefault));

            Popup.Attach(temp.Detach());
        }
    }

    bool PanelColumns::OnContextMenu(PanelHeader& header, CPoint const& pn)
    {
        RebuildPopup(PopupIdBase);

        int id = Popup.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, pn.x, pn.y, header);
        if (0 == id)
            return false;

        if (PopupIdClearSort == id)
        {
            ClearHeaderIcon(header, iHot);
            header.ClearSelected();
            iHot = -1;
            return true;
        }

        id -= PopupIdBase;
        if (MaxCount == id)
        {
            ResetColumnsToDefault();
            ResetHeader(header);
            return true;
        }

        else if (_IsValidId(id))
        {
            Used[id] = !Used[id];

            if (!Used[id] && (id == iHot))
                iHot = -1;

            ResetHeader(header);
            return true;
        }

        return false;
    }

    //static COLORREF GetItemColor(CTime const& currentTime, CTime const& itemTime, PanelViewColors const& colors)
    //{
    //    CTimeSpan tSpan = currentTime - itemTime;
    //    LONGLONG daysLeft = tSpan.GetDays();
    //
    //    if (daysLeft > 15)
    //        return colors.textColor;
    //
    //    else if (daysLeft > 8)
    //        return colors.prevWeekColor;
    //
    //    else if (daysLeft > 1)
    //        return colors.thisWeekColor;
    //
    //    else if (daysLeft > 0)
    //        return colors.yesterdaysColor;
    //
    //    return colors.todayColor;
    //}

    //static bool UseStockIcon(Fl::Entry const& info, CIconHandle& icon)
    //{
    //    if (info.IsUpperDir())
    //        icon.Attach(Icons.UpperDir);
    //
    //    return icon.m_hIcon != NULL;
    //}

//    static void SetTextAndBackground(WTL::CDC& dc, CRect const& rc, Fl::Entry const& info, int index
//        , bool hot
//        , bool selected
//        , bool panelActive
//        , CTime const& currentTime
//        , PanelViewColors const& colors
//        )
//    {
//        COLORREF tcolor = GetItemColor(currentTime, CFileTime(info.GetTime()), colors);
//
//        if (selected)
//            tcolor = Cf::IncreaseColor(Cf::ReduceColor(tcolor, 0, 100, 100), 80, 0, 0);
//
//// ##TODO: Configure dark or light theme"))
//        if (info.IsHidden()) 
//            tcolor = Cf::ReduceColor(tcolor, 63, 63, 63); 
//
//        int bi = (index % 2) & 1;
//        COLORREF bcolor = colors.background[bi];
//
//// ##TODO: Configure dark or light theme"))
//        if (hot)
//            bcolor = panelActive ? colors.hotItemBackgroundColor : Cf::ReduceColor(colors.hotItemBackgroundColor, 31, 31, 31);
//                                                                   
//
//        dc.FillSolidRect(rc, bcolor);
//        dc.SetTextColor(tcolor);
//    }

    //static void DrawEntryIcon(WTL::CDC& dc, CRect& rcRow, Fl::Entry const& info, CSize const& isize)
    //{
    //    CIcon shell;
    //    CIconHandle disp;
    //
    //    if (!UseStockIcon(info, disp))
    //    {
    //        UINT flags = 0
    //                   | SHGFI_ADDOVERLAYS | SHGFI_SMALLICON
    //                   | (info.IsReparsePoint() ? SHGFI_LINKOVERLAY : 0)
    //                   ;
    //
    //        shell.Attach(info.LoadShellIcon(flags));
    //        disp = shell;
    //    }
    //
    //    dc.DrawIconEx(rcRow.left, rcRow.top, disp, isize.cx, isize.cy, 0, NULL, DI_NORMAL);
    //    rcRow.right = rcRow.left + isize.cx;
    //}

    //void PanelColumns::DrawRow(CDC& dc, CRect const& rcEntry, Fl::Entry const& info, int index
    //    , CSize const& iconSz, bool selected, bool hot, bool panelActive, CTime const& currentTime, PanelViewColors const& colors) const
    //{
    //    CRect rcInner = rcEntry;
    //    rcInner.DeflateRect(1, 1);
    //
    //    SetTextAndBackground(dc, rcEntry, info, index, hot, selected, panelActive, currentTime, colors);
    //
    //    for (int i=0; i<MaxCount; i++)
    //    {
    //        if (Used[i])
    //        {
    //            rcInner.right = rcInner.left + Widths[i];
    //            Drawer[i](dc, rcInner, info, iconSz);
    //            rcInner.left = rcInner.right + 1;
    //        }
    //
    //        if (rcInner.left > rcEntry.right)
    //            break;
    //    }
    //}

    //static void NameDrawer(CDC& dc, CRect const& rc, Fl::Entry const& info, CSize const& sz)
    //{
    //    CRect rcIcon(rc.left + 2, rc.top, rc.right + 2, rc.bottom);
    //    DrawEntryIcon(dc, rcIcon, info, sz);
    //
    //    WidecharString text;
    //    if (info.IsDir())
    //        text = L"[" + info.GetFilename() + L"]";
    //
    //    else
    //        text = info.GetName().empty() ? info.GetExt() : info.GetName();
    //
    //    if (!text.empty())
    //    {
    //        CRect rcText = rc;
    //        rcText.left = rcIcon.right + 1;
    //
    //        ::DrawTextW(dc, text.c_str(), (int)text.length(), rcText
    //            , DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE);
    //    }
    //}

    //inline static void TextDraw(CDC& dc, CRect const& rc, std::wstring const& text, UINT flags)
    //{
    //    CRect rcText = rc;
    //    rcText.DeflateRect(3, 0);
    //    ::DrawTextW(dc, text.c_str(), (int)text.length(), rcText, flags);
    //}

    //static void TypeDrawer(CDC& dc, CRect const& rc, Fl::Entry const& info, CSize const& sz) 
    //{
    //    if (info.IsDir())
    //        return ;
    //
    //    if (info.GetName().empty())
    //        return ;
    //
    //    std::wstring text = info.GetExt();
    //
    //    if (text.length() > 2)
    //        TextDraw(dc, rc, std::wstring(&text[1]), DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE);
    //}

    //static void SizeDrawer(CDC& dc, CRect const& rc, Fl::Entry const& info, CSize const& sz) 
    //{
    //    if (info.IsDir() && (info.GetSize() < 1))
    //        return ;
    //
    //    WidecharString text = Str::HumanSize(info.GetSize());
    //
    //    CRect rcSize(rc.left + 1, rc.top, rc.right - 6, rc.bottom);
    //    TextDraw(dc, rcSize, text.c_str(), DT_RIGHT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE);
    //}

    //static void DateDrawer(CDC& dc, CRect const& rc, Fl::Entry const& info, CSize const& sz) 
    //{
    //    WidecharString text = CTime(CFileTime(info.GetTime())).Format(_LS(StrId_HMmdY));
    //    TextDraw(dc, rc, text, DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE);
    //}

    //static void ShellTypeDrawer(CDC& dc, CRect const& rc, Fl::Entry const& info, CSize const& sz)
    //{
    //    TextDraw(dc, rc, info.GetShellType(), DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE);
    //}

    //static void AttrsDrawer(CDC& dc, CRect const& rc, Fl::Entry const& info, CSize const& sz)
    //{
    //    if (info.GetNativeAttr())
    //        TextDraw(dc, rc, info.NativeAttrString().c_str(), DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE);
    //}

    //double PanelColumns::Sort(Fl::List& entries) const
    //{
    //    if (_IsValidId(iHot))
    //        return entries.Sort(Less[iHot], Ascending[iHot]);
    //
    //    return 0.;
    //}

    //Fl::List::LESSFN const& PanelColumns::GetCurrentLessFunction() const
    //{
    //    return _IsValidId(iHot) ? Less[iHot] : Less[0];
    //}

    //bool PanelColumns::GetCurrentAscending() const
    //{
    //    return _IsValidId(iHot) ? Ascending[iHot] : true;
    //}

    bool PanelColumns::DoSort(int id, PanelHeader& header, FSort& sorter, FItemVector const& items)
    {
        if (_IsValidId(id))
        {
            int iLast = iHot;
            ClearHeaderIcon(header, iLast);
    
            iHot = id;
    
            if (iLast == iHot)
                Ascending[iHot] = !Ascending[iHot];

            sorter.CreateIndex(iHot, Ascending[iHot], items);
    
            SetHeaderIconAsc(header, iHot, Ascending[iHot]);

            return true;
        }

        return false;
    }
}
