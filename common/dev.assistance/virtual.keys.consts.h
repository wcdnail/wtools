//------------------------------------------------------------------------
// Virtual keys...
// 
DA_NUM_TO_STR(VK_LBUTTON        ); // 0x01
DA_NUM_TO_STR(VK_RBUTTON        ); // 0x02
DA_NUM_TO_STR(VK_CANCEL         ); // 0x03
DA_NUM_TO_STR(VK_MBUTTON        ); // 0x04    // NOT contiguous with L & RBUTTON
#if(_WIN32_WINNT >= 0x0500)
DA_NUM_TO_STR(VK_XBUTTON1       ); // 0x05    // NOT contiguous with L & RBUTTON
DA_NUM_TO_STR(VK_XBUTTON2       ); // 0x06    // NOT contiguous with L & RBUTTON
#endif // _WIN32_WINNT >= 0x0500
DA_NUM_TO_STR(VK_BACK           ); // 0x08
DA_NUM_TO_STR(VK_TAB            ); // 0x09
DA_NUM_TO_STR(VK_CLEAR          ); // 0x0C
DA_NUM_TO_STR(VK_RETURN         ); // 0x0D
DA_NUM_TO_STR(VK_SHIFT          ); // 0x10
DA_NUM_TO_STR(VK_CONTROL        ); // 0x11
DA_NUM_TO_STR(VK_MENU           ); // 0x12
DA_NUM_TO_STR(VK_PAUSE          ); // 0x13
DA_NUM_TO_STR(VK_CAPITAL        ); // 0x14
DA_NUM_TO_STR(VK_KANA           ); // 0x15
DA_NUM_TO_STR(VK_JUNJA          ); // 0x17
DA_NUM_TO_STR(VK_FINAL          ); // 0x18
DA_NUM_TO_STR(VK_KANJI          ); // 0x19
DA_NUM_TO_STR(VK_ESCAPE         ); // 0x1B
DA_NUM_TO_STR(VK_CONVERT        ); // 0x1C
DA_NUM_TO_STR(VK_NONCONVERT     ); // 0x1D
DA_NUM_TO_STR(VK_ACCEPT         ); // 0x1E
DA_NUM_TO_STR(VK_MODECHANGE     ); // 0x1F
DA_NUM_TO_STR(VK_SPACE          ); // 0x20
DA_NUM_TO_STR(VK_PRIOR          ); // 0x21
DA_NUM_TO_STR(VK_NEXT           ); // 0x22
DA_NUM_TO_STR(VK_END            ); // 0x23
DA_NUM_TO_STR(VK_HOME           ); // 0x24
DA_NUM_TO_STR(VK_LEFT           ); // 0x25
DA_NUM_TO_STR(VK_UP             ); // 0x26
DA_NUM_TO_STR(VK_RIGHT          ); // 0x27
DA_NUM_TO_STR(VK_DOWN           ); // 0x28
DA_NUM_TO_STR(VK_SELECT         ); // 0x29
DA_NUM_TO_STR(VK_PRINT          ); // 0x2A
DA_NUM_TO_STR(VK_EXECUTE        ); // 0x2B
DA_NUM_TO_STR(VK_SNAPSHOT       ); // 0x2C
DA_NUM_TO_STR(VK_INSERT         ); // 0x2D
DA_NUM_TO_STR(VK_DELETE         ); // 0x2E
DA_NUM_TO_STR(VK_HELP           ); // 0x2F
DA_NUM_TO_STR(VK_LWIN           ); // 0x5B
DA_NUM_TO_STR(VK_RWIN           ); // 0x5C
DA_NUM_TO_STR(VK_APPS           ); // 0x5D
DA_NUM_TO_STR(VK_SLEEP          ); // 0x5F
DA_NUM_TO_STR(VK_NUMPAD0        ); // 0x60
DA_NUM_TO_STR(VK_NUMPAD1        ); // 0x61
DA_NUM_TO_STR(VK_NUMPAD2        ); // 0x62
DA_NUM_TO_STR(VK_NUMPAD3        ); // 0x63
DA_NUM_TO_STR(VK_NUMPAD4        ); // 0x64
DA_NUM_TO_STR(VK_NUMPAD5        ); // 0x65
DA_NUM_TO_STR(VK_NUMPAD6        ); // 0x66
DA_NUM_TO_STR(VK_NUMPAD7        ); // 0x67
DA_NUM_TO_STR(VK_NUMPAD8        ); // 0x68
DA_NUM_TO_STR(VK_NUMPAD9        ); // 0x69
DA_NUM_TO_STR(VK_MULTIPLY       ); // 0x6A
DA_NUM_TO_STR(VK_ADD            ); // 0x6B
DA_NUM_TO_STR(VK_SEPARATOR      ); // 0x6C
DA_NUM_TO_STR(VK_SUBTRACT       ); // 0x6D
DA_NUM_TO_STR(VK_DECIMAL        ); // 0x6E
DA_NUM_TO_STR(VK_DIVIDE         ); // 0x6F
DA_NUM_TO_STR(VK_F1             ); // 0x70
DA_NUM_TO_STR(VK_F2             ); // 0x71
DA_NUM_TO_STR(VK_F3             ); // 0x72
DA_NUM_TO_STR(VK_F4             ); // 0x73
DA_NUM_TO_STR(VK_F5             ); // 0x74
DA_NUM_TO_STR(VK_F6             ); // 0x75
DA_NUM_TO_STR(VK_F7             ); // 0x76
DA_NUM_TO_STR(VK_F8             ); // 0x77
DA_NUM_TO_STR(VK_F9             ); // 0x78
DA_NUM_TO_STR(VK_F10            ); // 0x79
DA_NUM_TO_STR(VK_F11            ); // 0x7A
DA_NUM_TO_STR(VK_F12            ); // 0x7B
DA_NUM_TO_STR(VK_F13            ); // 0x7C
DA_NUM_TO_STR(VK_F14            ); // 0x7D
DA_NUM_TO_STR(VK_F15            ); // 0x7E
DA_NUM_TO_STR(VK_F16            ); // 0x7F
DA_NUM_TO_STR(VK_F17            ); // 0x80
DA_NUM_TO_STR(VK_F18            ); // 0x81
DA_NUM_TO_STR(VK_F19            ); // 0x82
DA_NUM_TO_STR(VK_F20            ); // 0x83
DA_NUM_TO_STR(VK_F21            ); // 0x84
DA_NUM_TO_STR(VK_F22            ); // 0x85
DA_NUM_TO_STR(VK_F23            ); // 0x86
DA_NUM_TO_STR(VK_F24            ); // 0x87
DA_NUM_TO_STR(VK_NUMLOCK        ); // 0x90
DA_NUM_TO_STR(VK_SCROLL         ); // 0x91
DA_NUM_TO_STR(VK_OEM_NEC_EQUAL  ); // 0x92   // '=' key on numpad
DA_NUM_TO_STR(VK_OEM_FJ_MASSHOU ); // 0x93   // 'Unregister word' key
DA_NUM_TO_STR(VK_OEM_FJ_TOUROKU ); // 0x94   // 'Register word' key
DA_NUM_TO_STR(VK_OEM_FJ_LOYA    ); // 0x95   // 'Left OYAYUBI' key
DA_NUM_TO_STR(VK_OEM_FJ_ROYA    ); // 0x96   // 'Right OYAYUBI' key
DA_NUM_TO_STR(VK_LSHIFT         ); // 0xA0
DA_NUM_TO_STR(VK_RSHIFT         ); // 0xA1
DA_NUM_TO_STR(VK_LCONTROL       ); // 0xA2
DA_NUM_TO_STR(VK_RCONTROL       ); // 0xA3
DA_NUM_TO_STR(VK_LMENU          ); // 0xA4
DA_NUM_TO_STR(VK_RMENU          ); // 0xA5
#if(_WIN32_WINNT >= 0x0500)
DA_NUM_TO_STR(VK_BROWSER_BACK        ); // 0xA6
DA_NUM_TO_STR(VK_BROWSER_FORWARD     ); // 0xA7
DA_NUM_TO_STR(VK_BROWSER_REFRESH     ); // 0xA8
DA_NUM_TO_STR(VK_BROWSER_STOP        ); // 0xA9
DA_NUM_TO_STR(VK_BROWSER_SEARCH      ); // 0xAA
DA_NUM_TO_STR(VK_BROWSER_FAVORITES   ); // 0xAB
DA_NUM_TO_STR(VK_BROWSER_HOME        ); // 0xAC

DA_NUM_TO_STR(VK_VOLUME_MUTE         ); // 0xAD
DA_NUM_TO_STR(VK_VOLUME_DOWN         ); // 0xAE
DA_NUM_TO_STR(VK_VOLUME_UP           ); // 0xAF
DA_NUM_TO_STR(VK_MEDIA_NEXT_TRACK    ); // 0xB0
DA_NUM_TO_STR(VK_MEDIA_PREV_TRACK    ); // 0xB1
DA_NUM_TO_STR(VK_MEDIA_STOP          ); // 0xB2
DA_NUM_TO_STR(VK_MEDIA_PLAY_PAUSE    ); // 0xB3
DA_NUM_TO_STR(VK_LAUNCH_MAIL         ); // 0xB4
DA_NUM_TO_STR(VK_LAUNCH_MEDIA_SELECT ); // 0xB5
DA_NUM_TO_STR(VK_LAUNCH_APP1         ); // 0xB6
DA_NUM_TO_STR(VK_LAUNCH_APP2         ); // 0xB7

#endif // _WIN32_WINNT >= 0x0500
DA_NUM_TO_STR(VK_OEM_1          ); // 0xBA   // ';:' for US
DA_NUM_TO_STR(VK_OEM_PLUS       ); // 0xBB   // '+' any country
DA_NUM_TO_STR(VK_OEM_COMMA      ); // 0xBC   // ',' any country
DA_NUM_TO_STR(VK_OEM_MINUS      ); // 0xBD   // '-' any country
DA_NUM_TO_STR(VK_OEM_PERIOD     ); // 0xBE   // '.' any country
DA_NUM_TO_STR(VK_OEM_2          ); // 0xBF   // '/?' for US
DA_NUM_TO_STR(VK_OEM_3          ); // 0xC0   // '`~' for US
DA_NUM_TO_STR(VK_OEM_4          ); // 0xDB  //  '[{' for US
DA_NUM_TO_STR(VK_OEM_5          ); // 0xDC  //  '\|' for US
DA_NUM_TO_STR(VK_OEM_6          ); // 0xDD  //  ']}' for US
DA_NUM_TO_STR(VK_OEM_7          ); // 0xDE  //  ''"' for US
DA_NUM_TO_STR(VK_OEM_8          ); // 0xDF
DA_NUM_TO_STR(VK_OEM_AX         ); // 0xE1  //  'AX' key on Japanese AX kbd
DA_NUM_TO_STR(VK_OEM_102        ); // 0xE2  //  "<>" or "\|" on RT 102-key kbd.
DA_NUM_TO_STR(VK_ICO_HELP       ); // 0xE3  //  Help key on ICO
DA_NUM_TO_STR(VK_ICO_00         ); // 0xE4  //  00 key on ICO
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(VK_PROCESSKEY     ); // 0xE5
#endif // WINVER >= 0x0400
DA_NUM_TO_STR(VK_ICO_CLEAR      ); // 0xE6
#if(_WIN32_WINNT >= 0x0500)
DA_NUM_TO_STR(VK_PACKET         ); // 0xE7
#endif // _WIN32_WINNT >= 0x0500
DA_NUM_TO_STR(VK_OEM_RESET      ); // 0xE9
DA_NUM_TO_STR(VK_OEM_JUMP       ); // 0xEA
DA_NUM_TO_STR(VK_OEM_PA1        ); // 0xEB
DA_NUM_TO_STR(VK_OEM_PA2        ); // 0xEC
DA_NUM_TO_STR(VK_OEM_PA3        ); // 0xED
DA_NUM_TO_STR(VK_OEM_WSCTRL     ); // 0xEE
DA_NUM_TO_STR(VK_OEM_CUSEL      ); // 0xEF
DA_NUM_TO_STR(VK_OEM_ATTN       ); // 0xF0
DA_NUM_TO_STR(VK_OEM_FINISH     ); // 0xF1
DA_NUM_TO_STR(VK_OEM_COPY       ); // 0xF2
DA_NUM_TO_STR(VK_OEM_AUTO       ); // 0xF3
DA_NUM_TO_STR(VK_OEM_ENLW       ); // 0xF4
DA_NUM_TO_STR(VK_OEM_BACKTAB    ); // 0xF5
DA_NUM_TO_STR(VK_ATTN           ); // 0xF6
DA_NUM_TO_STR(VK_CRSEL          ); // 0xF7
DA_NUM_TO_STR(VK_EXSEL          ); // 0xF8
DA_NUM_TO_STR(VK_EREOF          ); // 0xF9
DA_NUM_TO_STR(VK_PLAY           ); // 0xFA
DA_NUM_TO_STR(VK_ZOOM           ); // 0xFB
DA_NUM_TO_STR(VK_NONAME         ); // 0xFC
DA_NUM_TO_STR(VK_PA1            ); // 0xFD
DA_NUM_TO_STR(VK_OEM_CLEAR      ); // 0xFE
