#include "stdafx.h"
#include "wtl.colorizer.control.details.h"

namespace Cf
{
    Colorizer::Details<WTL::CStatic>::AppearType Colorizer::Details<WTL::CStatic>::GetAppearType(HWND hwnd)
    {
        UINT result = Normal;
        UINT   type = static_cast<UINT>(::GetWindowLongW(hwnd, GWL_STYLE)) & SS_TYPEMASK;
        if (type >= SS_ICON && type <= SS_USERITEM) {
            result = Icon + (type - SS_ICON);
        }
        else if (type >= SS_OWNERDRAW && type <= SS_ETCHEDFRAME) {
            result = Ownerdraw + (type - SS_OWNERDRAW);
        }
        return (AppearType)result;
    }

    Colorizer::BorderFlags Colorizer::Details<WTL::CStatic>::GetBorderType(HWND hwnd)
    {
        LONG style = 0, estyle = 0;
        BorderFlags result = Colorizer::GetBorderType(hwnd, style, estyle);
        if (style & SS_SUNKEN) {
            return BorderSunken;
        }
        return result;
    }

    UINT Colorizer::Details<WTL::CStatic>::GetDrawTextFormat(HWND hwnd)
    {
        UINT  style = static_cast<UINT>(::GetWindowLong(hwnd, GWL_STYLE));
        UINT result = style & 3 |
                      DT_EXPANDTABS // TODO: check it
                      ;
        return result
                | (style & SS_CENTERIMAGE ? DT_VCENTER | DT_SINGLELINE : 0)
                | (style & SS_ENDELLIPSIS ? DT_END_ELLIPSIS : 0)
                | (style & SS_PATHELLIPSIS ? DT_PATH_ELLIPSIS : 0)
                | (style & SS_WORDELLIPSIS ? DT_WORD_ELLIPSIS : 0)
                ;
    }

    Colorizer::Details<WTL::CButton>::AppearType Colorizer::Details<WTL::CButton>::GetAppearType(HWND hwnd)
    {
        UINT style = static_cast<UINT>(::GetWindowLong(hwnd, GWL_STYLE));
        AppearType result = static_cast<AppearType>(style & BS_TYPEMASK);
        if (BS_FLAT & style) {
            result = Flat;
        }
        if (BS_PUSHLIKE & style) {
            result = Normal;
        }
        return result; 
    }

    Colorizer::BorderFlags Colorizer::Details<WTL::CButton>::GetBorderType(HWND hwnd)
    {
        LONG style = 0, estyle = 0;
        return Colorizer::GetBorderType(hwnd, style, estyle); 
    }

    UINT Colorizer::Details<WTL::CButton>::GetDrawTextFormat(HWND hwnd)
    {
        UINT style = static_cast<UINT>(::GetWindowLong(hwnd, GWL_STYLE));
        UINT type = style & BS_TYPEMASK;

        UINT ha = BS_CENTER & style;
        UINT va = BS_VCENTER & style;

        UINT defaultHa = static_cast<UINT>((type < Checkbox) || (Flat == type) ? DT_CENTER : DT_LEFT);

        return (style & BS_MULTILINE ? 0 : DT_SINGLELINE)
                | (!ha ? defaultHa : (BS_CENTER == ha ? DT_CENTER : (BS_RIGHT == ha ? DT_RIGHT : DT_LEFT)))
                | (!va ? DT_VCENTER : (BS_VCENTER == va ? DT_VCENTER : (BS_BOTTOM == va ? DT_BOTTOM : DT_TOP)))
                ;
    }

    Colorizer::Details<WTL::CComboBox>::AppearType Colorizer::Details<WTL::CComboBox>::GetAppearType(HWND hwnd)
    {
        AppearType result = static_cast<AppearType>(::GetWindowLong(hwnd, GWL_STYLE) & 3 - 1);
        return result; 
    }

    Colorizer::BorderFlags Colorizer::Details<WTL::CComboBox>::GetBorderType(HWND hwnd)
    {
        LONG style = 0, estyle = 0;
        return Colorizer::GetBorderType(hwnd, style, estyle); 
    }

    UINT Colorizer::Details<WTL::CComboBox>::GetDrawTextFormat(HWND)
    { 
        return DT_CENTER | DT_VCENTER | DT_SINGLELINE; 
    }
}
