#include "stdafx.h"
#include "status.ui.h"
#include "status.bar.ex.h"
#include <string.utils.format.h>
#include <string.utils.error.code.h>
#include <atlconv.h>

namespace Twins
{
    Ui::StatusBar& UiStatus()
    {
        static Ui::StatusBar statusBar;
        return statusBar;
    }

    static void _AddStatusMessage(HRESULT hr, HICON icon, PCWSTR message)
    {
        if (0 != hr)
        {
            UiStatus().AddFormat(icon, L"%s - %d %s", message, hr, Str::ErrorCode<>::SystemMessage(hr));
            UiStatus().Blink();
        }
        else
            UiStatus().Add(message, icon);
    }

    void SetMainframeStatus(HRESULT hr, HICON icon, PCWSTR format, ...)
    {
        if (NULL != format)
        {
            va_list ap;
            va_start(ap, format);
            CStringW message = Str::Elipsis<wchar_t>::FormatV(format, ap);
            _AddStatusMessage(hr, icon, message);
            va_end(ap);
        }
    }

    void SetMainframeStatus(HRESULT hr, HICON icon, PCSTR format, ...)
    {
        if (NULL != format)
        {
            va_list ap;
            va_start(ap, format);
            CStringA message = Str::Elipsis<char>::FormatV(format, ap);
            _AddStatusMessage(hr, icon, CA2W(message, CP_UTF8));
            va_end(ap);
        }
    }
}
