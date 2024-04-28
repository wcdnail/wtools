#pragma once

#ifdef _WIN32
#include <winuser.h>
#include <atltrace.h>
#endif

namespace Keyboard
{
    namespace Code
    {
#ifdef _WIN32
        static const unsigned Alt = VK_MENU;
        static const unsigned Control = VK_CONTROL;
        static const unsigned CapsLock = VK_CAPITAL;
        static const unsigned NumLock = VK_NUMLOCK;
        static const unsigned ScrollLock = VK_SCROLL;
        static const unsigned Shift = VK_SHIFT;
#endif
    }

    namespace Impl
    {
#ifdef _WIN32
        inline USHORT GetState(USHORT mask, unsigned code) 
        {
            USHORT state = (USHORT)::GetKeyState(code);
            return mask & state; 
        }

        inline USHORT GetAsyncState(USHORT mask, unsigned code) 
        {
            USHORT state = (USHORT)::GetAsyncKeyState(code);
            return mask & state; 
        }

        inline USHORT GetPressedState(unsigned code) { return GetAsyncState(0x8000, code); }
        inline USHORT GetOnState(unsigned code) { return GetState(0x0001, code);
        }
#endif
    }

    inline bool IsPressed(unsigned code) { return 0 != Impl::GetPressedState(code); }
    inline bool IsOn(unsigned code) { return 0 != Impl::GetOnState(code); }

    inline bool IsAltPressed() { return IsPressed(Code::Alt); }
    inline bool IsCtrlPressed() { return IsPressed(Code::Control); }
    inline bool IsShiftPressed() { return IsPressed(Code::Shift); }
    inline bool IsCapsLockOn() { return IsOn(Code::CapsLock); }
    inline bool IsNumLockOn() { return IsOn(Code::NumLock); }
    inline bool IsScrollLockOn() { return IsOn(Code::ScrollLock); }
}
