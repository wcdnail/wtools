#pragma once

#include "wcdafx.api.h"
#include "wtl.colorizer.h"

namespace Cf
{
    #pragma region General

    template <typename U>
    struct Colorizer::Details 
    {
        enum AppearType : UINT { Normal = 0 };

        static AppearType GetAppearType(HWND) 
        { 
            AppearType result = Normal;
            return result; 
        }

        static BorderFlags GetBorderType(HWND hwnd) 
        {
            LONG style = 0, estyle = 0;
            return Colorizer::GetBorderType(hwnd, style, estyle); 
        }

        static UINT GetDrawTextFormat(HWND) 
        { 
            return DT_CENTER | DT_VCENTER | DT_SINGLELINE; 
        }
    };

    #pragma endregion 

    #pragma region Static details

    template <>
    struct Colorizer::Details<WTL::CStatic> 
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
    struct Colorizer::Details<WTL::CButton> 
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
    struct Colorizer::Details<WTL::CComboBox>
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
