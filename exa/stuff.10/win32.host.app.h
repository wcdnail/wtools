#pragma once

namespace Simple
{
    int AppEntry(HINSTANCE instance, HINSTANCE = NULL, LPTSTR = NULL, int = SW_SHOW);
    void DropFailBox(PCTSTR caption, PCTSTR format, ...);
}
