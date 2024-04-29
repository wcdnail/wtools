#pragma once

#include "wtl.colorizer.control.h"
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlw.h>
#include <atlctrlx.h>

namespace CF::Colorized
{
    using             ZStatic = WTL::CStaticT<ControlBase>;
    using             ZButton = WTL::CButtonT<ControlBase>;
    using          ZScrollBar = WTL::CScrollBarT<ControlBase>;
    using           ZComboBox = WTL::CComboBoxT<ControlBase>;
    using               ZEdit = WTL::CEditT<ControlBase>;
    using            ZListBox = WTL::CListBoxT<ControlBase>;
    using         ZHeaderCtrl = WTL::CHeaderCtrlT<ControlBase>;
    using           ZLinkCtrl = WTL::CLinkCtrlT<ControlBase>;
    using       ZListViewCtrl = WTL::CListViewCtrlT<ControlBase>;
    using       ZTreeViewCtrl = WTL::CTreeViewCtrlT<ControlBase>;
    using         ZComboBoxEx = WTL::CComboBoxExT<ControlBase>;
    using            ZTabCtrl = WTL::CTabCtrlT<ControlBase>;
    using      ZIPAddressCtrl = WTL::CIPAddressCtrlT<ControlBase>;
    using          ZPagerCtrl = WTL::CPagerCtrlT<ControlBase>;
    using    ZProgressBarCtrl = WTL::CProgressBarCtrlT<ControlBase>;
    using       ZTrackBarCtrl = WTL::CTrackBarCtrlT<ControlBase>;
    using         ZUpDownCtrl = WTL::CUpDownCtrlT<ControlBase>;
    using ZDateTimePickerCtrl = WTL::CDateTimePickerCtrlT<ControlBase>;
    using  ZMonthCalendarCtrl = WTL::CMonthCalendarCtrlT<ControlBase>;
    using       ZRichEditCtrl = WTL::CRichEditCtrlT<ControlBase>;

    template <typename T>
    struct SpecImpl
    {
        SpecImpl(SpecImpl const&) = delete;
        SpecImpl& operator = (SpecImpl const&) = delete;

        ~SpecImpl();
        SpecImpl();
    };

    template <typename T>
    inline SpecImpl<T>::~SpecImpl()
    {
    }

    template <typename T>
    inline SpecImpl<T>::SpecImpl()
    {
    }

    template <>
    struct SpecImpl<ZComboBox>
    {
        //Control<ZListBox> m_ListBox;
        ControlPtr m_ListBoxPtr;

        SpecImpl(SpecImpl const&) = delete;
        SpecImpl& operator = (SpecImpl const&) = delete;

        ~SpecImpl();
        SpecImpl();
    };

    inline SpecImpl<ZComboBox>::~SpecImpl()
    {
    }

    inline SpecImpl<ZComboBox>::SpecImpl()
        : m_ListBoxPtr()
    {
    }
}
