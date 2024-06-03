#ifndef _DH_debug_console_2_h__
#define _DH_debug_console_2_h__

#include "../debug.console/debug.console.h"

namespace Dh
{
    class _PUBLIC DebugConsole2: private DebugConsole
    {
    public:
        static DebugConsole2& Instance(); /* throw (std::bad_alloc) */

        using DebugConsole::ReceiveDebugOutput;
        using DebugConsole::ReceiveStdOutput;
        using DebugConsole::SetParameters;
        using DebugConsole::SetAutoScroll;
        using DebugConsole::Show;
        using DebugConsole::Hide;
        using DebugConsole::Clean;
        using DebugConsole::SetTitleText;
        using DebugConsole::Puts;
        using DebugConsole::AskPathAndSave;
        using DebugConsole::Save;
        using DebugConsole::Destroy;

    private:
        DebugConsole2(); /* throw (std::bad_alloc) */
        ~DebugConsole2();
        DebugConsole2(DebugConsole2 const&);
        DebugConsole2& operator = (DebugConsole2 const&);
    };
}

#endif // _DH_debug_console_2_h__
