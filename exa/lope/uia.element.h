#pragma once

#include <uiautomation.h>
#include <atlcomcli.h>
#include <atlstr.h>
#include <atltypes.h>

namespace Uia
{
    typedef CComPtr<IUIAutomation> AutomationPtr;
    typedef CComPtr<IUIAutomationElement> ElementPtr;

    struct Element
    {
        ElementPtr Pointee;
        HWND NativeHandle;
        CRect Rect;
        CONTROLTYPEID TypeId;
        CPoint Pt;

        Element();
        Element(ElementPtr const& elm);
        Element(AutomationPtr const& automation, POINT pt);

        bool operator ! () const { return Pointee.operator!(); }
        operator bool() const { return !(!Pointee); }
        bool operator != (Element const& rhs) const;
        Element& operator = (Element const& rhs);

        CStringW StringDump() const;
        std::wstring GetName() const;
        std::wstring GetHelpText() const;

    private:
        void Init();
    };

    inline Element::Element()
        : Pointee()
        , NativeHandle(NULL)
        , Rect()
        , TypeId((CONTROLTYPEID)-1)
        , Pt()
    {}

    inline Element::Element(ElementPtr const& elm)
        : Pointee(elm)
        , NativeHandle(NULL)
        , Rect()
        , TypeId((CONTROLTYPEID)-1)
        , Pt()
    {
        Init();
    }

    inline Element::Element(AutomationPtr const& automation, POINT pt)
        : Pointee()
        , NativeHandle(NULL)
        , Rect()
        , TypeId((CONTROLTYPEID)-1)
        , Pt(pt)
    {
        CComPtr<IUIAutomationElement> elm;
        HRESULT hr = automation->ElementFromPoint(pt, &elm);

        if (SUCCEEDED(hr))
        {
            Pointee = elm;
            Init();
        }
    }

    inline Element& Element::operator = (Element const& rhs)
    {
        if (this != &rhs)
        {
            Pointee = rhs.Pointee;
            NativeHandle = rhs.NativeHandle;
            Rect = rhs.Rect;
            TypeId = rhs.TypeId;
            Pt = rhs.Pt;
        }

        return *this;
    }

    inline void Element::Init()
    {
        if (Pointee)
        {
            UIA_HWND uiHwnd = NULL;
            if (SUCCEEDED(Pointee->get_CurrentNativeWindowHandle(&uiHwnd)))
                NativeHandle = (HWND)uiHwnd;

            CONTROLTYPEID typeId = 0;
            if (SUCCEEDED(Pointee->get_CurrentControlType(&typeId)))
                TypeId = typeId;

            CRect rcTemp;
            if (SUCCEEDED(Pointee->get_CurrentBoundingRectangle(rcTemp)))
                Rect = rcTemp;
        }
    }

    inline std::wstring Element::GetName() const
    {
        if (Pointee)
        {
            BSTR text;
            if (SUCCEEDED(Pointee->get_CurrentName(&text)))
                return std::wstring(text);
        }
        
        return std::wstring();
    }

    inline std::wstring Element::GetHelpText() const
    {
        if (Pointee)
        {
            BSTR text;
            if (SUCCEEDED(Pointee->get_CurrentHelpText(&text)))
                return std::wstring(text);
        }

        return std::wstring();
    }

    inline PCWSTR _ControlTypeString(CONTROLTYPEID id)
    {
        switch (id)
        {
        case UIA_ButtonControlTypeId	    : return L"Button";
        case UIA_CalendarControlTypeId	    : return L"Calendar";
        case UIA_CheckBoxControlTypeId	    : return L"CheckBox";
        case UIA_ComboBoxControlTypeId	    : return L"ComboBox";
        case UIA_EditControlTypeId	        : return L"Edit";
        case UIA_HyperlinkControlTypeId	    : return L"Hyperlink";
        case UIA_ImageControlTypeId	        : return L"Image";
        case UIA_ListItemControlTypeId	    : return L"ListItem";
        case UIA_ListControlTypeId	        : return L"List";
        case UIA_MenuControlTypeId	        : return L"Menu";
        case UIA_MenuBarControlTypeId	    : return L"MenuBar";
        case UIA_MenuItemControlTypeId	    : return L"MenuItem";
        case UIA_ProgressBarControlTypeId	: return L"ProgressBar";
        case UIA_RadioButtonControlTypeId	: return L"RadioButton";
        case UIA_ScrollBarControlTypeId	    : return L"ScrollBar";
        case UIA_SliderControlTypeId	    : return L"Slider";
        case UIA_SpinnerControlTypeId	    : return L"Spinner";
        case UIA_StatusBarControlTypeId	    : return L"StatusBar";
        case UIA_TabControlTypeId	        : return L"Tab";
        case UIA_TabItemControlTypeId	    : return L"TabItem";
        case UIA_TextControlTypeId	        : return L"Text";
        case UIA_ToolBarControlTypeId	    : return L"ToolBar";
        case UIA_ToolTipControlTypeId	    : return L"ToolTip";
        case UIA_TreeControlTypeId	        : return L"Tree";
        case UIA_TreeItemControlTypeId	    : return L"TreeItem";
        case UIA_CustomControlTypeId	    : return L"Custom";
        case UIA_GroupControlTypeId	        : return L"Group";
        case UIA_ThumbControlTypeId	        : return L"Thumb";
        case UIA_DataGridControlTypeId	    : return L"DataGrid";
        case UIA_DataItemControlTypeId	    : return L"DataItem";
        case UIA_DocumentControlTypeId	    : return L"Document";
        case UIA_SplitButtonControlTypeId	: return L"SplitButton";
        case UIA_WindowControlTypeId	    : return L"Window";
        case UIA_PaneControlTypeId	        : return L"Pane";
        case UIA_HeaderControlTypeId	    : return L"Header";
        case UIA_HeaderItemControlTypeId	: return L"HeaderItem";
        case UIA_TableControlTypeId	        : return L"Table";
        case UIA_TitleBarControlTypeId	    : return L"TitleBar";
        case UIA_SeparatorControlTypeId	    : return L"Separator";
        case UIA_SemanticZoomControlTypeId	: return L"SemanticZoom";
        }

        return L"Unknown";
    }

    inline CStringW Element::StringDump() const
    {
        CStringW result;
        result.Format(L"%p (%06d [%s])", NativeHandle, TypeId, _ControlTypeString(TypeId));
        return result;
    }

    inline bool Element::operator != (Element const& rhs) const
    {
        return Rect.PtInRect(rhs.Pt) ? TypeId != rhs.TypeId : true;
    }
}

