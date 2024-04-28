#pragma once

#if 0
#include "config.h"
#include <reflection/ref.logfont.h>
#include <reflection/ref.brush.h>
#include <reflection/ref.pen.h>
#include <atlgdi.h>
#include <atluser.h>

namespace Twins
{
#if 0
    COLORREF ReduceColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B);
    COLORREF IncreaseColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B);
    COLORREF InvertColor(COLORREF color);
#endif

    class ThemeResources;

    ThemeResources& Theme();

    class ThemeResources
    {
    public:
        struct StockIcons
        {
            CIcon Main;
            CIcon UpperDir;
            CIcon RedFloppy;
            CIcon LinkOverlay;
            CIcon Confirm;
            CIcon Cancel;
            CIcon HeaderAsc;
            CIcon HeaderDesc;

            StockIcons(HINSTANCE resModule);
            void Reload();
        };

        struct StaticCtl
        {
            int bkMode;
            COLORREF textColor;
            CBrush background;

            StaticCtl();
            void Reload(int mode, COLORREF text, COLORREF bk);
        };

        struct PanelTab
        {
            COLORREF textColor;
            COLORREF hotTextColor;
            COLORREF selTextColor;
            COLORREF backColor[2];
            COLORREF backHotColor[2];
            COLORREF backSelColor[2];

            PanelTab();
            void Reload();
        };

        struct PanelView
        {
            COLORREF textColor;
            COLORREF todayColor;
            COLORREF yesterdaysColor;
            COLORREF thisWeekColor;
            COLORREF prevWeekColor;
            COLORREF hotItemTextColor;
            COLORREF hotItemBackgroundColor;
            COLORREF backgroundColor;
            COLORREF busyBackgroundColor;
            COLORREF selectedTextColor;
            COLORREF background[2];

            PanelView();
            void Reload();
        };

        StockIcons Icons;
        CFont defaultFont;
        CFont fixedFont;
        CFont fixedBoldFont;
        CFont panelFont;
        PanelView panelView;
        PanelTab panelTab;
        StaticCtl pathHint[2];

        void OnChanged();

    private:
        Config::Storage config_;
        Config::Section fonst_;
        Ref::Font defaultRefFont_;
        Ref::Font fixedRefFont_;
        Ref::Font panelRefFont_;

        friend ThemeResources& Theme();

        ThemeResources();
        ~ThemeResources();

        ThemeResources(ThemeResources const&);
        ThemeResources& operator = (ThemeResources const&);
    };
}
#endif
