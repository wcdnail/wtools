#pragma once

#include <color.stuff.h>
#include <atlgdi.h>
#include <wingdi.h>
#include <boost/noncopyable.hpp>

namespace Twins
{
    class Appearance: boost::noncopyable
    {
    public:
        COLORREF TextColor;
        COLORREF TodayTextColor;
        COLORREF YesterdayTextColor;
        COLORREF WeekTextColor;
        COLORREF PrevWeekTextColor;
        COLORREF HotItemBackgroundColor;
        COLORREF BackgroundColor;
        COLORREF BusyBackgroundColor;
        CBrush BackgroundBrush;

        COLORREF CommandLineTextColor;
        COLORREF CommandLineBackColor;
        CBrush CommandLineBackBrush;

        CFont Font;
        CFont PanelFont;
        CFont DriveBarFont;
        CFont DriveComboFont;
        CFont PathBarFont;
        CFont CommandLineFont;

        COLORREF ItemBackgroundColor[2];

    public:
        Appearance();
        ~Appearance();
    };

    inline Appearance::Appearance()
        : TextColor             (0x707070)
        , TodayTextColor        (0xfff0f0)
        , YesterdayTextColor    (0xe0e0e0)
        , WeekTextColor         (0xcf9090)
        , PrevWeekTextColor     (0x9f8080)
        , HotItemBackgroundColor(0x3c3c3c)
        , BackgroundColor       (0x050505)
        , BusyBackgroundColor   (0x000000)
        , BackgroundBrush       (::CreateSolidBrush(BackgroundColor))

        , CommandLineTextColor  (0xdfdfdf)
        , CommandLineBackColor  (0x202020)
        , CommandLineBackBrush  (::CreateSolidBrush(CommandLineBackColor))

        , Font                  (::CreateFontW(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, L"Tahoma"))
        , PanelFont             (::CreateFontW(-13, 0, 0, 0, FW_MEDIUM, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, L"Courier New"))
        , DriveBarFont          (::CreateFontW(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, L"Tahoma"))
        , DriveComboFont        (::CreateFontW(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, L"Courier New"))
        , PathBarFont           (::CreateFontW(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, L"Tahoma"))
        , CommandLineFont       (::CreateFontW(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, L"Courier New"))
    {
        ItemBackgroundColor[0] = BackgroundColor;
        ItemBackgroundColor[1] = Cf::ReduceColor(ItemBackgroundColor[0], 8, 8, 5);
    }

    inline Appearance::~Appearance()
    {}
}
