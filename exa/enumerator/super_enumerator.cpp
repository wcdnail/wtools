#include "stdafx.h"
#include "mpr.network.h"
#include "lm.network.h"
#include "auto.timer.h"

static void WaitEscape()
{
    ::fputws(L"\n\nTap `esc` for exit...\n", stderr);

    while (true)
    {
        if (::_kbhit())
        {
            int kcode = ::_getch();
            if (27 == kcode)
                break;
        }
    }
}

int main(int argc, char* argv[])
{
    bool showAll = true;

    {
        Cf::Timer timer;
        ::wprintf(L"\n\n::NetServerEnum ---\n");
        Net::EnumerateDomains();
        ::wprintf(L"\n%.8f sec.\n", timer.Seconds());
    }

    {
        Cf::Timer timer;
        ::wprintf(L"\n\n::WNetOpenEnum ---\n");
        Mpr::Enumerate(NULL, RESOURCE_GLOBALNET, RESOURCETYPE_ANY, 0, 0, showAll);
        ::wprintf(L"\n%.8f sec.\n", timer.Seconds());
    }

    WaitEscape();
    return 0;
}
