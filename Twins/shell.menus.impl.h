#pragma once

/////////////////////////////////////////////////////////////////////
// class to show shell contextmenu of files/folders/shell objects
// developed by R. Engels 2003
// adopted in 2012
/////////////////////////////////////////////////////////////////////

#include <wcdafx.api.h>
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
        class ContextMenu
        {
        public:
            DELETE_COPY_MOVE_OF(ContextMenu);

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
