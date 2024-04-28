#include "stdafx.h"
#ifdef _MSC_VER
#  pragma warning(disable: 4820) // 4820: 'WTL::tagCBRPOPUPMENU': '4' bytes padding added after data member 'WTL::tagCBRPOPUPMENU::cbSize'
#  pragma warning(disable: 4365) // 4365: 'argument': conversion from 'unsigned int' to 'int', signed/unsigned mismatch
#  pragma warning(disable: 4514) // 4514: 'WTL::CCommandBarCtrlBase::CCommandBarCtrlBase': unreferenced inline function has been removed
#endif
#include "wtl.colorizer.control.specific.h"

namespace CF::Colorized
{
    template <typename T>
    SpecImpl<T>::~SpecImpl()
    {
    }

    template <typename T>
    SpecImpl<T>::SpecImpl()
    {
    }

    SpecImpl<ZComboBox>::~SpecImpl()
    {
    }

    SpecImpl<ZComboBox>::SpecImpl()
        : m_ListBox()
    {
    }

    template struct SpecImpl<ZStatic>;
    template struct SpecImpl<ZButton>;
    template struct SpecImpl<ZScrollBar>;
    template struct SpecImpl<ZComboBox>;
    template struct SpecImpl<ZEdit>;
    template struct SpecImpl<ZListBox>;
    template struct SpecImpl<ZHeaderCtrl>;
    template struct SpecImpl<ZLinkCtrl>;
    template struct SpecImpl<ZListViewCtrl>;
    template struct SpecImpl<ZTreeViewCtrl>;
    template struct SpecImpl<ZComboBoxEx>;
    template struct SpecImpl<ZTabCtrl>;
    template struct SpecImpl<ZIPAddressCtrl>;
    template struct SpecImpl<ZPagerCtrl>;
    template struct SpecImpl<ZProgressBarCtrl>;
    template struct SpecImpl<ZTrackBarCtrl>;
    template struct SpecImpl<ZUpDownCtrl>;
    template struct SpecImpl<ZDateTimePickerCtrl>;
    template struct SpecImpl<ZMonthCalendarCtrl>;
    template struct SpecImpl<ZRichEditCtrl>;
}
