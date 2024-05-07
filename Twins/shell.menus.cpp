#include "stdafx.h"
#include "shell.menus.h"
#include "shell.menus.impl.h"
#include <dh.tracing.h>

namespace Twins
{
    unsigned TrackShellContextMenu(CWindow owner, CPoint point, Fl::List const& files)
    {
        ShellImpl::ContextMenu menu;
        unsigned rv = (SUCCEEDED(menu.SetObjects(files)) ? menu.ShowContextMenu(owner, point) : (unsigned)-1);
        DH::TPrintf(_T("ShellMnu: return value = %d\n"), rv);
        return rv;
    }
}
