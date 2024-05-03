#include "stdafx.h"
#include "win32.host.app.h"
#include <crash.report/app.entry.adapters.h>

int AppEntry(HINSTANCE instance)
{
    return Simple::AppEntry(instance);
}
