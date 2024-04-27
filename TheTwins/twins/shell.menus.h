#pragma once

#include <atlwin.h>
#include <atltypes.h>

namespace Fl
{
    class List;
}

namespace Twins
{
    unsigned TrackShellContextMenu(CWindow owner, CPoint point, Fl::List const& files);
}
