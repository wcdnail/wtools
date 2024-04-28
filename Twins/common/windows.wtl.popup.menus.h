#pragma once

#include <atlwin.h>
#include <atltypes.h>
#include <atluser.h>
#include <boost/noncopyable.hpp>

namespace Cf
{
    class PopupMenu: boost::noncopyable
    {
    public:
        PopupMenu();
        PopupMenu(int); // do nothing
        ~PopupMenu();

        template <class Container, class StringPred>
        int Load(int startId, Container const& cont, StringPred const& spred);

        int Show(CPoint p, ATL::CWindow owner, unsigned flags, LPTPMPARAMS lptpm = NULL);
        int Show(ATL::CWindow control, ATL::CWindow owner, unsigned flags, LPTPMPARAMS lptpm = NULL);
        CString GetString(int n, unsigned flags) const;

        void Add(CString const& itemtext, int id, UINT flags = 0);
        void AddSeparator(UINT flags = 0);

        bool IsValid() const;
        void CreatePopup();

    private:
        WTL::CMenu popup_;

        CPoint GetPosition(ATL::CWindow const& control);
    };

    inline PopupMenu::PopupMenu()
        : popup_()
    {
        CreatePopup();
    }

    inline PopupMenu::PopupMenu(int)
        : popup_()
    {}

    inline PopupMenu::~PopupMenu()
    {}

    inline bool PopupMenu::IsValid() const
    {
        return NULL != popup_.m_hMenu;
    }

    inline void PopupMenu::CreatePopup()
    {
        popup_.CreatePopupMenu();
    }

    template <class Container, class StringPred>
    inline int PopupMenu::Load(int startId, Container const& cont, StringPred const& spred)
    {
        int counter = 0;

        for (typename Container::const_iterator it = cont.begin(); it != cont.end(); ++it)
            popup_.AppendMenu(MF_STRING, startId + counter++, spred(*it));

        return counter;
    }

    inline CPoint PopupMenu::GetPosition(ATL::CWindow const& control)
    {
        CRect rc;
        control.GetWindowRect(rc);
// ##TODO: Select position by flags"))
        return CPoint(rc.left, rc.bottom);
    }

    inline int PopupMenu::Show(ATL::CWindow control, ATL::CWindow owner, unsigned flags, LPTPMPARAMS lptpm /*= NULL*/)
    {
        CPoint p = GetPosition(control);
        int rv = Show(p, owner, flags, lptpm);
        return rv;
    }

    inline int PopupMenu::Show(CPoint p, ATL::CWindow owner, unsigned flags, LPTPMPARAMS lptpm /*= NULL*/)
    {
        int rv = (int)popup_.TrackPopupMenuEx(flags, p.x, p.y, owner, lptpm);
        return rv;
    }

    inline CString PopupMenu::GetString(int n, unsigned flags) const
    {
        CString rv;
        if (popup_.GetMenuString(n, rv, flags) > 0)
            return rv;

        return _T("");
    }

    inline void PopupMenu::Add(CString const& itemtext, int id, UINT flags /*= 0*/)
    {
        popup_.AppendMenu(MF_STRING | flags, (UINT_PTR)id, itemtext);
    }

    inline void PopupMenu::AddSeparator(UINT flags/* = 0*/)
    {
        popup_.AppendMenu(MF_SEPARATOR | flags);
    }
}

