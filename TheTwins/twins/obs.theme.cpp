#include "stdafx.h"
#if 0
#include "theme.h"
#include "res/resource.h"
#include <color.stuff.h>

namespace Twins
{
    ThemeResources::StockIcons::StockIcons(HINSTANCE resModule)
        : Main          (::LoadIcon(resModule, MAKEINTRESOURCE(IDR_MAINFRAME)))
        , UpperDir      (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_UPPERDIR)))
        , RedFloppy     (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_REDFLOPPY)))
        , LinkOverlay   (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_LINKOVERLAY)))
        , Confirm       (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_CONFIRM)))
        , Cancel        (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_CANCEL)))
        , HeaderAsc     (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_HEADER_UP)))
        , HeaderDesc    (::LoadIcon(resModule, MAKEINTRESOURCE(IDI_HEADER_DOWN)))
    {}

    void ThemeResources::StockIcons::Reload()
    {}

    ThemeResources::PanelTab::PanelTab()
        : textColor(0)
        , hotTextColor(0)
        , selTextColor(0)
    {
        backColor[0] = 0xf8e0ca;
        backColor[1] = 0xeb7a87;
        backHotColor[0] = backColor[0];
        backHotColor[1] = backColor[1];
        backSelColor[0] = 0xf1dafd;
        backSelColor[1] = 0x89337e;
    }

    void ThemeResources::PanelTab::Reload()
    {
        textColor = ::GetSysColor(COLOR_WINDOWTEXT);
        backColor[0] = ::GetSysColor(COLOR_MENU);
        backColor[1] = Cf::ReduceColor(backColor[0], 64, 64, 64);

        hotTextColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
        backHotColor[0] = ::GetSysColor(COLOR_HIGHLIGHT);
        backHotColor[1] = Cf::ReduceColor(backHotColor[0], 64, 64, 64);

        selTextColor = textColor;
        backSelColor[0] = backColor[1];
        backSelColor[1] = backColor[0];
    }

    ThemeResources& Theme()
    {
        static ThemeResources inst;
        return inst;
    }

    ThemeResources::StaticCtl::StaticCtl()
        : bkMode(0)
        , textColor(0)
        , background(0)
    {}

    ThemeResources::PanelView::PanelView()
        : textColor(0)
        , todayColor(0xff341e)
        , yesterdaysColor(0xcf04a6)
        , thisWeekColor(0x913c35)
        , prevWeekColor(0x541914)
        , hotItemTextColor(0xffffff)
        , hotItemBackgroundColor(0)
        , backgroundColor(0xffffff)
        , busyBackgroundColor(0x1f1f1f)
        , selectedTextColor(0x1f1fff)
    {
        background[0] = 0xffffff;
        background[1] = 0xffffff;
    }

    void ThemeResources::StaticCtl::Reload(int mode, COLORREF text, COLORREF bk)
    {
        bkMode = mode;
        textColor = text;
        background.Attach(::CreateSolidBrush(bk));
    }

    void ThemeResources::PanelView::Reload()
    {
        textColor       = ::GetSysColor(COLOR_WINDOWTEXT);

        todayColor      = Cf::IncreaseColor(textColor, 40, 40, 200);
        yesterdaysColor = Cf::IncreaseColor(todayColor, 0, 100, 0);
        thisWeekColor   = Cf::IncreaseColor(yesterdaysColor, 100, -100, 0);
        prevWeekColor   = Cf::IncreaseColor(thisWeekColor, 50, 40, 0);

        hotItemTextColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
        hotItemBackgroundColor = ::GetSysColor(COLOR_HIGHLIGHT);
        backgroundColor = ::GetSysColor(COLOR_WINDOW);
        background[0] = backgroundColor;
        background[1] = Cf::ReduceColor(background[0], 8, 8, 5);
        busyBackgroundColor = 0;
    }

    ThemeResources::ThemeResources()
        : Icons(ModuleHelper::GetResourceInstance())
        , defaultFont(NULL) 
        , fixedFont(NULL)
        , fixedBoldFont(NULL)
        , panelFont(NULL)
        , panelView()
        , panelTab()
        , config_("twins_appearance.xml", "Theme")
        , fonst_(config_.GetRoot(), "Fonts")
        , defaultRefFont_(-11, _T("Tahoma"))
        , fixedRefFont_(-12, _T("Courier New"))
        , panelRefFont_(-13, _T("Courier New"))
    {
        fonst_["Default"] = defaultRefFont_;
        fonst_["Fixed"] = fixedRefFont_;
        fonst_["Panel"] = panelRefFont_;
        //config_.Load();

        defaultFont.Attach(::CreateFontIndirect(&defaultRefFont_));
        fixedFont.Attach(::CreateFontIndirect(&fixedRefFont_));
        panelFont.Attach(::CreateFontIndirect(&panelRefFont_));

        LOGFONT fixedBold = fixedRefFont_;
        fixedBold.lfWeight = FW_HEAVY;
        fixedBoldFont.Attach(::CreateFontIndirect(&fixedBold));
        
        OnChanged();
    }

    ThemeResources::~ThemeResources()
    {
        //config_.Save();
    }

    void ThemeResources::OnChanged()
    {
        {
            COLORREF back = GetSysColor(COLOR_ACTIVECAPTION);
            COLORREF text = ::GetSysColor(COLOR_CAPTIONTEXT);
            pathHint[0].Reload(TRANSPARENT, text, back);

            back = ::GetSysColor(COLOR_INACTIVECAPTION);
            text = ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);
            pathHint[1].Reload(TRANSPARENT, text, back);
        }

        panelView.Reload();
        panelTab.Reload();
    }

#if 0
    COLORREF ReduceColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B)
    {
        return RGB(GetRValue(color) < R ? 0 : GetRValue(color) - R,
                   GetGValue(color) < G ? 0 : GetGValue(color) - G,
                   GetBValue(color) < B ? 0 : GetBValue(color) - B);
    }

    COLORREF IncreaseColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B)
    {
        return RGB(min(GetRValue(color) + R, 255),
                   min(GetGValue(color) + G, 255),
                   min(GetBValue(color) + B, 255));
    }

    COLORREF InvertColor(COLORREF color)
    {
        return RGB(255 - GetRValue(color),
                   255 - GetGValue(color),
                   255 - GetBValue(color));
    }
#endif
}
#endif
