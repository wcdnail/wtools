#pragma once

#include "wtl.colorizer.control.specific.h"

namespace CF::Colorized
{
    enum BorderFlags : UINT;

#pragma region General

    template <typename U>
    struct Details 
    {
        enum AppearType : UINT { Normal = 0 };

        static AppearType GetAppearType(HWND);
        static BorderFlags GetBorderType(HWND hwnd);
        static UINT GetDrawTextFormat(HWND);
    };

#pragma endregion 
#pragma region Static details

    template <>
    struct Details<ZStatic> 
    {
        enum AppearType: UINT
        {
            Normal = 0,
            Icon,
            Blackrect,
            Grayrect,
            Whiterect,
            Blackframe,
            Grayframe,
            Whiteframe,
            Useritem,
            Ownerdraw,
            Bitmap,
            Enhmetafile,
            Etchedhorz,
            Etchedvert,
            Etchedframe,
        };

        static AppearType GetAppearType(HWND hwnd);
        static BorderFlags GetBorderType(HWND hwnd);
        static UINT GetDrawTextFormat(HWND hwnd);
    };

#pragma endregion 
#pragma region Button details

    template <>
    struct Details<ZButton> 
    {
        enum AppearType : UINT
        {
            Normal = 0,
            PushButton = Normal,
            DefPushButton,
            Checkbox,
            AutoCheckbox,
            RadioButton,
            ThreeState,
            Auto3State,
            Groupbox,
            UserButton,
            AutoRadioButton,
            Pushbox,
            Ownerdraw,
            Flat,
        };

        static AppearType GetAppearType(HWND hwnd);
        static BorderFlags GetBorderType(HWND hwnd);
        static UINT GetDrawTextFormat(HWND hwnd);
    };

#pragma endregion 
#pragma region Combo details

    template <>
    struct Details<ZComboBox>
    {
        enum AppearType: UINT
        {
          Normal = 0 
        , Simple = Normal
        , Dropdown
        , DropdownList
        };

        static AppearType GetAppearType(HWND hwnd);
        static BorderFlags GetBorderType(HWND hwnd);
        static UINT GetDrawTextFormat(HWND);
    };

#pragma endregion 
}
