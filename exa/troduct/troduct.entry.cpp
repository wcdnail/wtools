#include "stdafx.h"
#include <niceday/debug.trace.h>

namespace App 
{
    int EntryPoint()
    {
        ::DebugTraceA("What a Nice Day...\n");

        ::Sleep(100);
        ::DebugTraceA("What a Nice Day... #2\n");

        ::Sleep(50);
        ::DebugTraceA("What a Nice Day... #3\n");

        ::Sleep(15);
        ::DebugTraceA("%s (%d) `%s`\n", "What a Nice Day... #4", 256, "trololo...");

        return 0;
    }
}
