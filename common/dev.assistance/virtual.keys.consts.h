//////////////////////////////////////////////////////////////////////////
// Virtual keys...
// 
        _ON_CONST(VK_LBUTTON        ); // 0x01
        _ON_CONST(VK_RBUTTON        ); // 0x02
        _ON_CONST(VK_CANCEL         ); // 0x03
        _ON_CONST(VK_MBUTTON        ); // 0x04    // NOT contiguous with L & RBUTTON
#if(_WIN32_WINNT >= 0x0500)
        _ON_CONST(VK_XBUTTON1       ); // 0x05    // NOT contiguous with L & RBUTTON
        _ON_CONST(VK_XBUTTON2       ); // 0x06    // NOT contiguous with L & RBUTTON
#endif // _WIN32_WINNT >= 0x0500
        _ON_CONST(VK_BACK           ); // 0x08
        _ON_CONST(VK_TAB            ); // 0x09
        _ON_CONST(VK_CLEAR          ); // 0x0C
        _ON_CONST(VK_RETURN         ); // 0x0D
        _ON_CONST(VK_SHIFT          ); // 0x10
        _ON_CONST(VK_CONTROL        ); // 0x11
        _ON_CONST(VK_MENU           ); // 0x12
        _ON_CONST(VK_PAUSE          ); // 0x13
        _ON_CONST(VK_CAPITAL        ); // 0x14
        _ON_CONST(VK_KANA           ); // 0x15
        _ON_CONST(VK_JUNJA          ); // 0x17
        _ON_CONST(VK_FINAL          ); // 0x18
        _ON_CONST(VK_KANJI          ); // 0x19
        _ON_CONST(VK_ESCAPE         ); // 0x1B
        _ON_CONST(VK_CONVERT        ); // 0x1C
        _ON_CONST(VK_NONCONVERT     ); // 0x1D
        _ON_CONST(VK_ACCEPT         ); // 0x1E
        _ON_CONST(VK_MODECHANGE     ); // 0x1F
        _ON_CONST(VK_SPACE          ); // 0x20
        _ON_CONST(VK_PRIOR          ); // 0x21
        _ON_CONST(VK_NEXT           ); // 0x22
        _ON_CONST(VK_END            ); // 0x23
        _ON_CONST(VK_HOME           ); // 0x24
        _ON_CONST(VK_LEFT           ); // 0x25
        _ON_CONST(VK_UP             ); // 0x26
        _ON_CONST(VK_RIGHT          ); // 0x27
        _ON_CONST(VK_DOWN           ); // 0x28
        _ON_CONST(VK_SELECT         ); // 0x29
        _ON_CONST(VK_PRINT          ); // 0x2A
        _ON_CONST(VK_EXECUTE        ); // 0x2B
        _ON_CONST(VK_SNAPSHOT       ); // 0x2C
        _ON_CONST(VK_INSERT         ); // 0x2D
        _ON_CONST(VK_DELETE         ); // 0x2E
        _ON_CONST(VK_HELP           ); // 0x2F
        _ON_CONST(VK_LWIN           ); // 0x5B
        _ON_CONST(VK_RWIN           ); // 0x5C
        _ON_CONST(VK_APPS           ); // 0x5D
        _ON_CONST(VK_SLEEP          ); // 0x5F
        _ON_CONST(VK_NUMPAD0        ); // 0x60
        _ON_CONST(VK_NUMPAD1        ); // 0x61
        _ON_CONST(VK_NUMPAD2        ); // 0x62
        _ON_CONST(VK_NUMPAD3        ); // 0x63
        _ON_CONST(VK_NUMPAD4        ); // 0x64
        _ON_CONST(VK_NUMPAD5        ); // 0x65
        _ON_CONST(VK_NUMPAD6        ); // 0x66
        _ON_CONST(VK_NUMPAD7        ); // 0x67
        _ON_CONST(VK_NUMPAD8        ); // 0x68
        _ON_CONST(VK_NUMPAD9        ); // 0x69
        _ON_CONST(VK_MULTIPLY       ); // 0x6A
        _ON_CONST(VK_ADD            ); // 0x6B
        _ON_CONST(VK_SEPARATOR      ); // 0x6C
        _ON_CONST(VK_SUBTRACT       ); // 0x6D
        _ON_CONST(VK_DECIMAL        ); // 0x6E
        _ON_CONST(VK_DIVIDE         ); // 0x6F
        _ON_CONST(VK_F1             ); // 0x70
        _ON_CONST(VK_F2             ); // 0x71
        _ON_CONST(VK_F3             ); // 0x72
        _ON_CONST(VK_F4             ); // 0x73
        _ON_CONST(VK_F5             ); // 0x74
        _ON_CONST(VK_F6             ); // 0x75
        _ON_CONST(VK_F7             ); // 0x76
        _ON_CONST(VK_F8             ); // 0x77
        _ON_CONST(VK_F9             ); // 0x78
        _ON_CONST(VK_F10            ); // 0x79
        _ON_CONST(VK_F11            ); // 0x7A
        _ON_CONST(VK_F12            ); // 0x7B
        _ON_CONST(VK_F13            ); // 0x7C
        _ON_CONST(VK_F14            ); // 0x7D
        _ON_CONST(VK_F15            ); // 0x7E
        _ON_CONST(VK_F16            ); // 0x7F
        _ON_CONST(VK_F17            ); // 0x80
        _ON_CONST(VK_F18            ); // 0x81
        _ON_CONST(VK_F19            ); // 0x82
        _ON_CONST(VK_F20            ); // 0x83
        _ON_CONST(VK_F21            ); // 0x84
        _ON_CONST(VK_F22            ); // 0x85
        _ON_CONST(VK_F23            ); // 0x86
        _ON_CONST(VK_F24            ); // 0x87
        _ON_CONST(VK_NUMLOCK        ); // 0x90
        _ON_CONST(VK_SCROLL         ); // 0x91
        _ON_CONST(VK_OEM_NEC_EQUAL  ); // 0x92   // '=' key on numpad
        _ON_CONST(VK_OEM_FJ_MASSHOU ); // 0x93   // 'Unregister word' key
        _ON_CONST(VK_OEM_FJ_TOUROKU ); // 0x94   // 'Register word' key
        _ON_CONST(VK_OEM_FJ_LOYA    ); // 0x95   // 'Left OYAYUBI' key
        _ON_CONST(VK_OEM_FJ_ROYA    ); // 0x96   // 'Right OYAYUBI' key
        _ON_CONST(VK_LSHIFT         ); // 0xA0
        _ON_CONST(VK_RSHIFT         ); // 0xA1
        _ON_CONST(VK_LCONTROL       ); // 0xA2
        _ON_CONST(VK_RCONTROL       ); // 0xA3
        _ON_CONST(VK_LMENU          ); // 0xA4
        _ON_CONST(VK_RMENU          ); // 0xA5
#if(_WIN32_WINNT >= 0x0500)
        _ON_CONST(VK_BROWSER_BACK        ); // 0xA6
        _ON_CONST(VK_BROWSER_FORWARD     ); // 0xA7
        _ON_CONST(VK_BROWSER_REFRESH     ); // 0xA8
        _ON_CONST(VK_BROWSER_STOP        ); // 0xA9
        _ON_CONST(VK_BROWSER_SEARCH      ); // 0xAA
        _ON_CONST(VK_BROWSER_FAVORITES   ); // 0xAB
        _ON_CONST(VK_BROWSER_HOME        ); // 0xAC

        _ON_CONST(VK_VOLUME_MUTE         ); // 0xAD
        _ON_CONST(VK_VOLUME_DOWN         ); // 0xAE
        _ON_CONST(VK_VOLUME_UP           ); // 0xAF
        _ON_CONST(VK_MEDIA_NEXT_TRACK    ); // 0xB0
        _ON_CONST(VK_MEDIA_PREV_TRACK    ); // 0xB1
        _ON_CONST(VK_MEDIA_STOP          ); // 0xB2
        _ON_CONST(VK_MEDIA_PLAY_PAUSE    ); // 0xB3
        _ON_CONST(VK_LAUNCH_MAIL         ); // 0xB4
        _ON_CONST(VK_LAUNCH_MEDIA_SELECT ); // 0xB5
        _ON_CONST(VK_LAUNCH_APP1         ); // 0xB6
        _ON_CONST(VK_LAUNCH_APP2         ); // 0xB7

#endif // _WIN32_WINNT >= 0x0500
        _ON_CONST(VK_OEM_1          ); // 0xBA   // ';:' for US
        _ON_CONST(VK_OEM_PLUS       ); // 0xBB   // '+' any country
        _ON_CONST(VK_OEM_COMMA      ); // 0xBC   // ',' any country
        _ON_CONST(VK_OEM_MINUS      ); // 0xBD   // '-' any country
        _ON_CONST(VK_OEM_PERIOD     ); // 0xBE   // '.' any country
        _ON_CONST(VK_OEM_2          ); // 0xBF   // '/?' for US
        _ON_CONST(VK_OEM_3          ); // 0xC0   // '`~' for US
        _ON_CONST(VK_OEM_4          ); // 0xDB  //  '[{' for US
        _ON_CONST(VK_OEM_5          ); // 0xDC  //  '\|' for US
        _ON_CONST(VK_OEM_6          ); // 0xDD  //  ']}' for US
        _ON_CONST(VK_OEM_7          ); // 0xDE  //  ''"' for US
        _ON_CONST(VK_OEM_8          ); // 0xDF
        _ON_CONST(VK_OEM_AX         ); // 0xE1  //  'AX' key on Japanese AX kbd
        _ON_CONST(VK_OEM_102        ); // 0xE2  //  "<>" or "\|" on RT 102-key kbd.
        _ON_CONST(VK_ICO_HELP       ); // 0xE3  //  Help key on ICO
        _ON_CONST(VK_ICO_00         ); // 0xE4  //  00 key on ICO
#if(WINVER >= 0x0400)
        _ON_CONST(VK_PROCESSKEY     ); // 0xE5
#endif // WINVER >= 0x0400
        _ON_CONST(VK_ICO_CLEAR      ); // 0xE6
#if(_WIN32_WINNT >= 0x0500)
        _ON_CONST(VK_PACKET         ); // 0xE7
#endif // _WIN32_WINNT >= 0x0500
        _ON_CONST(VK_OEM_RESET      ); // 0xE9
        _ON_CONST(VK_OEM_JUMP       ); // 0xEA
        _ON_CONST(VK_OEM_PA1        ); // 0xEB
        _ON_CONST(VK_OEM_PA2        ); // 0xEC
        _ON_CONST(VK_OEM_PA3        ); // 0xED
        _ON_CONST(VK_OEM_WSCTRL     ); // 0xEE
        _ON_CONST(VK_OEM_CUSEL      ); // 0xEF
        _ON_CONST(VK_OEM_ATTN       ); // 0xF0
        _ON_CONST(VK_OEM_FINISH     ); // 0xF1
        _ON_CONST(VK_OEM_COPY       ); // 0xF2
        _ON_CONST(VK_OEM_AUTO       ); // 0xF3
        _ON_CONST(VK_OEM_ENLW       ); // 0xF4
        _ON_CONST(VK_OEM_BACKTAB    ); // 0xF5
        _ON_CONST(VK_ATTN           ); // 0xF6
        _ON_CONST(VK_CRSEL          ); // 0xF7
        _ON_CONST(VK_EXSEL          ); // 0xF8
        _ON_CONST(VK_EREOF          ); // 0xF9
        _ON_CONST(VK_PLAY           ); // 0xFA
        _ON_CONST(VK_ZOOM           ); // 0xFB
        _ON_CONST(VK_NONAME         ); // 0xFC
        _ON_CONST(VK_PA1            ); // 0xFD
        _ON_CONST(VK_OEM_CLEAR      ); // 0xFE
