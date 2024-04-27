#pragma once

#include "wtl.colorizer.h"

namespace Cf
{
    #pragma region General

    template <typename U>
    struct Colorizer::Details 
    {
        enum AppearType { Normal = 0 };

        static AppearType GetAppearType(HWND) 
        { 
            unsigned result = Normal;
            return (AppearType)result; 
        }

        static BorderFlags GetBorderType(HWND hwnd) 
        {
            unsigned style = 0, estyle = 0;
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
        enum AppearType
        {
          Normal = 0
        , Icon       
        , Blackrect  
        , Grayrect   
        , Whiterect  
        , Blackframe 
        , Grayframe  
        , Whiteframe 
        , Useritem
        , Ownerdraw  
        , Bitmap     
        , Enhmetafile
        , Etchedhorz 
        , Etchedvert 
        , Etchedframe
        };

        static AppearType GetAppearType(HWND hwnd)
        {
            unsigned result = Normal;

            unsigned type = ::GetWindowLong(hwnd, GWL_STYLE) & SS_TYPEMASK;

            if (type >= SS_ICON && type <= SS_USERITEM)
                result = Icon + (type - SS_ICON);

            else if (type >= SS_OWNERDRAW && type <= SS_ETCHEDFRAME)
                result = Ownerdraw + (type - SS_OWNERDRAW);

            return (AppearType)result;
        }

        static BorderFlags GetBorderType(HWND hwnd)
        {
            unsigned style = 0, estyle = 0;
            unsigned result = Colorizer::GetBorderType(hwnd, style, estyle);

            if (style & SS_SUNKEN)
                return BorderSunken;

            return (BorderFlags)result;
        }

        static UINT GetDrawTextFormat(HWND hwnd) 
        {
            unsigned style = ::GetWindowLong(hwnd, GWL_STYLE);

            UINT result = style & 3 
                        | DT_EXPANDTABS // TODO: check it
                        ; 

            return result
                | (style & SS_CENTERIMAGE ? DT_VCENTER | DT_SINGLELINE : 0)
                | (style & SS_ENDELLIPSIS ? DT_END_ELLIPSIS : 0)
                | (style & SS_PATHELLIPSIS ? DT_PATH_ELLIPSIS : 0)
                | (style & SS_WORDELLIPSIS ? DT_WORD_ELLIPSIS : 0)
                ;
        }
    };

    #pragma endregion 

    #pragma region Button details

    template <>
    struct Colorizer::Details<WTL::CButton> 
    {
        enum AppearType 
        {
          Normal = 0 
        , PushButton = Normal
        , DefPushButton
        , Checkbox
        , AutoCheckbox
        , RadioButton
        , ThreeState
        , Auto3State
        , Groupbox
        , UserButton
        , AutoRadioButton
        , Pushbox
        , Ownerdraw
        , Flat
        };

        static AppearType GetAppearType(HWND hwnd) 
        {
            unsigned style = ::GetWindowLong(hwnd, GWL_STYLE);
            unsigned result = style & BS_TYPEMASK;

            if (BS_FLAT & style)
                result = Flat;

            if (BS_PUSHLIKE & style)
                result = Normal;

            return (AppearType)result; 
        }

        static BorderFlags GetBorderType(HWND hwnd) 
        {
            unsigned style = 0, estyle = 0;
            return Colorizer::GetBorderType(hwnd, style, estyle); 
        }

        static UINT GetDrawTextFormat(HWND hwnd) 
        {
            unsigned style = ::GetWindowLong(hwnd, GWL_STYLE);
            unsigned type = style & BS_TYPEMASK;

            unsigned ha = BS_CENTER & style;
            unsigned va = BS_VCENTER & style;

            unsigned defaultHa = (type < Checkbox) || (Flat == type) ? DT_CENTER : DT_LEFT;

            return (style & BS_MULTILINE ? 0 : DT_SINGLELINE)
                 | (!ha ? defaultHa : (BS_CENTER == ha ? DT_CENTER : (BS_RIGHT == ha ? DT_RIGHT : DT_LEFT)))
                 | (!va ? DT_VCENTER : (BS_VCENTER == va ? DT_VCENTER : (BS_BOTTOM == va ? DT_BOTTOM : DT_TOP)))
                 ;
        }
    };

    #pragma endregion 

    #pragma region Combo details

    template <>
    struct Colorizer::Details<WTL::CComboBox>
    {
        enum AppearType 
        {
          Normal = 0 
        , Simple = Normal
        , Dropdown
        , DropdownList
        };

        static AppearType GetAppearType(HWND hwnd)
        { 
            unsigned result = ::GetWindowLong(hwnd, GWL_STYLE) & 3 - 1;
            return (AppearType)result; 
        }

        static BorderFlags GetBorderType(HWND hwnd) 
        {
            unsigned style = 0, estyle = 0;
            return Colorizer::GetBorderType(hwnd, style, estyle); 
        }

        static UINT GetDrawTextFormat(HWND) 
        { 
            return DT_CENTER | DT_VCENTER | DT_SINGLELINE; 
        }
    };

    #pragma endregion 
}
