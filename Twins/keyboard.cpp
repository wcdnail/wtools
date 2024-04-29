#include "stdafx.h"
#include "keyboard.h"
#include "command.key.ids.h"
#include <dh.tracing.h>

namespace Twins
{
    namespace Keyboard
    {
        KeyState::~KeyState()
        {}

        KeyState::KeyState(unsigned code, unsigned flags, bool shift, bool alt, bool ctrl)
            : Shift(shift)
            , Alt(alt)
            , Ctrl(ctrl)
            , Code(code)
            , Flags(flags)
        {}

        KeyState::KeyState(unsigned code/* = 0*/, unsigned flags/* = 0*/)
            : Shift(0 != (0x8000 & ::GetAsyncKeyState(VK_SHIFT)))
            , Alt(0 != (0x8000 & ::GetAsyncKeyState(VK_MENU)))
            , Ctrl(0 != (0x8000 & ::GetAsyncKeyState(VK_CONTROL)))
            , Code(code)
            , Flags(flags)
        {}

        void KeyState::Dump() const
        {
#ifdef _DEBUG_DUMP_KEY
            DH::ThreadPrintf(_T("KeyState: %08x `%s`\n"), KeyId(), KeyIdName().c_str());
#endif
        }

        unsigned KeyState::KeyId() const
        {
            return Command::KeyId::MakeId(Code, 0
                | (Shift ? Command::KeyId::Shift : 0)
                | (Alt   ? Command::KeyId::Alt : 0)
                | (Ctrl  ? Command::KeyId::Ctrl : 0)
                );
        }

        std::wstring KeyState::KeyIdName() const
        {
            std::wstring rv;

            if (Ctrl)
                rv = L"Ctrl";

            if (Alt)
            {
                if (!rv.empty())
                    rv += L"+";

                rv += L"Alt";
            }

            if (Shift)
            {
                if (!rv.empty())
                    rv += L"+";

                rv += L"Shift";
            }

            if (!rv.empty())
                rv += L" ";

            rv += CodeString(Code);
            return rv;
        }

        wchar_t const* CodeString(unsigned code)
        {
            switch (code)
            {
            case VK_BACK                : return L"Back";
            case VK_TAB                 : return L"Tab";
            case VK_CLEAR               : return L"Clear";
            case VK_RETURN              : return L"Return";
            case VK_SHIFT               : return L"Shift";
            case VK_CONTROL             : return L"Control";
            case VK_MENU                : return L"Menu";
            case VK_PAUSE               : return L"Pause"; 
            case VK_CAPITAL             : return L"Capital"; 
            case VK_KANA                : return L"Kana"; 
            case VK_JUNJA               : return L"Junja"; 
            case VK_FINAL               : return L"Final"; 
            case VK_KANJI               : return L"Kanji"; 
            case VK_ESCAPE              : return L"Escape"; 
            case VK_CONVERT             : return L"Convert"; 
            case VK_NONCONVERT          : return L"Nonconvert"; 
            case VK_ACCEPT              : return L"Accept"; 
            case VK_MODECHANGE          : return L"Modechange"; 
            case VK_SPACE               : return L"Space";
            case VK_PRIOR               : return L"Prior"; 
            case VK_NEXT                : return L"Next"; 
            case VK_END                 : return L"End"; 
            case VK_HOME                : return L"Home"; 
            case VK_LEFT                : return L"Left"; 
            case VK_UP                  : return L"Up"; 
            case VK_RIGHT               : return L"Right"; 
            case VK_DOWN                : return L"Down"; 
            case VK_SELECT              : return L"Select"; 
            case VK_PRINT               : return L"Print"; 
            case VK_EXECUTE             : return L"Execute"; 
            case VK_SNAPSHOT            : return L"Snapshot"; 
            case VK_INSERT              : return L"Insert"; 
            case VK_DELETE              : return L"Delete"; 
            case VK_HELP                : return L"Help"; 
            case VK_LWIN                : return L"LWin"; 
            case VK_RWIN                : return L"RWin"; 
            case VK_APPS                : return L"Apps"; 
            case VK_SLEEP               : return L"Sleep"; 
            case VK_NUMPAD0             : return L"Numpad0"; 
            case VK_NUMPAD1             : return L"Numpad1"; 
            case VK_NUMPAD2             : return L"Numpad2"; 
            case VK_NUMPAD3             : return L"Numpad3"; 
            case VK_NUMPAD4             : return L"Numpad4"; 
            case VK_NUMPAD5             : return L"Numpad5"; 
            case VK_NUMPAD6             : return L"Numpad6"; 
            case VK_NUMPAD7             : return L"Numpad7"; 
            case VK_NUMPAD8             : return L"Numpad8"; 
            case VK_NUMPAD9             : return L"Numpad9"; 
            case VK_MULTIPLY            : return L"Multiply"; 
            case VK_ADD                 : return L"Add"; 
            case VK_SEPARATOR           : return L"Separator"; 
            case VK_SUBTRACT            : return L"Subtract"; 
            case VK_DECIMAL             : return L"Decimal"; 
            case VK_DIVIDE              : return L"Divide"; 
            case VK_F1                  : return L"F1"; 
            case VK_F2                  : return L"F2"; 
            case VK_F3                  : return L"F3"; 
            case VK_F4                  : return L"F4"; 
            case VK_F5                  : return L"F5"; 
            case VK_F6                  : return L"F6"; 
            case VK_F7                  : return L"F7"; 
            case VK_F8                  : return L"F8"; 
            case VK_F9                  : return L"F9"; 
            case VK_F10                 : return L"F10"; 
            case VK_F11                 : return L"F11"; 
            case VK_F12                 : return L"F12"; 
            case VK_F13                 : return L"F13"; 
            case VK_F14                 : return L"F14"; 
            case VK_F15                 : return L"F15"; 
            case VK_F16                 : return L"F16"; 
            case VK_F17                 : return L"F17"; 
            case VK_F18                 : return L"F18"; 
            case VK_F19                 : return L"F19"; 
            case VK_F20                 : return L"F20"; 
            case VK_F21                 : return L"F21"; 
            case VK_F22                 : return L"F22"; 
            case VK_F23                 : return L"F23"; 
            case VK_F24                 : return L"F24"; 
            case VK_NUMLOCK             : return L"Numlock"; 
            case VK_SCROLL              : return L"Scroll"; 
            case VK_OEM_NEC_EQUAL       : return L"OemNecEqual"; 
            case VK_OEM_FJ_MASSHOU      : return L"OemFjMasshou"; 
            case VK_OEM_FJ_TOUROKU      : return L"OemFjTouroku"; 
            case VK_OEM_FJ_LOYA         : return L"OemFjLoya"; 
            case VK_OEM_FJ_ROYA         : return L"OemFjRoya"; 
            case VK_LSHIFT              : return L"LShift"; 
            case VK_RSHIFT              : return L"RShift"; 
            case VK_LCONTROL            : return L"LControl"; 
            case VK_RCONTROL            : return L"RControl"; 
            case VK_LMENU               : return L"LAlt"; 
            case VK_RMENU               : return L"RAlt"; 
#if(_WIN32_WINNT >= 0x0500)
            case VK_BROWSER_BACK        : return L"BrowserBack"; 
            case VK_BROWSER_FORWARD     : return L"BrowserForward"; 
            case VK_BROWSER_REFRESH     : return L"BrowserRefresh"; 
            case VK_BROWSER_STOP        : return L"BrowserStop"; 
            case VK_BROWSER_SEARCH      : return L"BrowserSearch"; 
            case VK_BROWSER_FAVORITES   : return L"BrowserFavorites"; 
            case VK_BROWSER_HOME        : return L"BrowserHome"; 
            case VK_VOLUME_MUTE         : return L"VolumeMute"; 
            case VK_VOLUME_DOWN         : return L"VolumeDown"; 
            case VK_VOLUME_UP           : return L"VolumeUp"; 
            case VK_MEDIA_NEXT_TRACK    : return L"MediaNextTrack"; 
            case VK_MEDIA_PREV_TRACK    : return L"MediaPrevTrack"; 
            case VK_MEDIA_STOP          : return L"MediaStop"; 
            case VK_MEDIA_PLAY_PAUSE    : return L"MediaPlayPause"; 
            case VK_LAUNCH_MAIL         : return L"LaunchMail"; 
            case VK_LAUNCH_MEDIA_SELECT : return L"LaunchMediaSelect"; 
            case VK_LAUNCH_APP1         : return L"LaunchApp1"; 
            case VK_LAUNCH_APP2         : return L"LaunchApp2"; 
#endif // _WIN32_WINNT >= 0x0500
            case VK_OEM_1               : return L"Oem1"; 
            case VK_OEM_PLUS            : return L"OemPlus"; 
            case VK_OEM_COMMA           : return L"OemComma"; 
            case VK_OEM_MINUS           : return L"OemMinus"; 
            case VK_OEM_PERIOD          : return L"OemPeriod"; 
            case VK_OEM_2               : return L"Oem2"; 
            case VK_OEM_3               : return L"Oem3"; 
            case VK_OEM_4               : return L"Oem4"; 
            case VK_OEM_5               : return L"Oem5"; 
            case VK_OEM_6               : return L"Oem6"; 
            case VK_OEM_7               : return L"Oem7"; 
            case VK_OEM_8               : return L"Oem8"; 
            case VK_OEM_AX              : return L"OemAx"; 
            case VK_OEM_102             : return L"Oem102"; 
            case VK_ICO_HELP            : return L"IcoHelp"; 
            case VK_ICO_00              : return L"Ico00"; 
#if(WINVER >= 0x0400)
            case VK_PROCESSKEY          : return L"ProcessKey"; 
#endif // WINVER >= 0x0400
            case VK_ICO_CLEAR           : return L"IcoClear"; 
#if(_WIN32_WINNT >= 0x0500)
            case VK_PACKET              : return L"Packet"; 
#endif // _WIN32_WINNT >= 0x0500
            case VK_OEM_RESET           : return L"OemReset"; 
            case VK_OEM_JUMP            : return L"OemJump"; 
            case VK_OEM_PA1             : return L"OemPa1"; 
            case VK_OEM_PA2             : return L"OemPa2"; 
            case VK_OEM_PA3             : return L"OemPa3"; 
            case VK_OEM_WSCTRL          : return L"OemWsctrl"; 
            case VK_OEM_CUSEL           : return L"OemCusel"; 
            case VK_OEM_ATTN            : return L"OemAttn"; 
            case VK_OEM_FINISH          : return L"OemFinish"; 
            case VK_OEM_COPY            : return L"OemCopy"; 
            case VK_OEM_AUTO            : return L"OemAuto"; 
            case VK_OEM_ENLW            : return L"OemEnlw"; 
            case VK_OEM_BACKTAB         : return L"OemBacktab"; 
            case VK_ATTN                : return L"Attn"; 
            case VK_CRSEL               : return L"Crsel"; 
            case VK_EXSEL               : return L"Exsel"; 
            case VK_EREOF               : return L"Ereof"; 
            case VK_PLAY                : return L"Play"; 
            case VK_ZOOM                : return L"Zoom"; 
            case VK_NONAME              : return L"Noname"; 
            case VK_PA1                 : return L"Pa1"; 
            case VK_OEM_CLEAR           : return L"OemClear"; 
            }

            static wchar_t character[2] = { 0, 0 };
            character[0] = (wchar_t)code;
            return character;
        }
    }
}
