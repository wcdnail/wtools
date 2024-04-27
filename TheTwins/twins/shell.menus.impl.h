#pragma once

/////////////////////////////////////////////////////////////////////
// class to show shell contextmenu of files/folders/shell objects
// developed by R. Engels 2003
// adopted by M. Nikonov 2012.
/////////////////////////////////////////////////////////////////////

#include <boost/noncopyable.hpp>
#include <atlstr.h>
#include <atluser.h>
#include <atlcomcli.h>
#include <shobjidl.h>

namespace Fl
{
    class List;
}

namespace Twins
{
    namespace ShellImpl
    {
        class ContextMenu: boost::noncopyable
        {
        public:
            ContextMenu();
            ~ContextMenu();

            CMenu& GetMenu();
            HRESULT SetObjects(Fl::List const& files);
            UINT ShowContextMenu(CWindow owner, CPoint pt);

        private:
            int count_;
            CMenu popup_;
            CComPtr<IShellFolder> ifolder_;
            LPITEMIDLIST* idls_;

            BOOL GetContextMenu(CComPtr<IContextMenu>& contextMenu, int& type);
        };
    }
}
