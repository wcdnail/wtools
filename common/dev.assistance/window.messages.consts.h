//------------------------------------------------------------------------
//
// https://learn.microsoft.com/en-us/windows/win32/winmsg/about-messages-and-message-queues#system-defined-messages
//
//------------------------------------------------------------------------
// Window messages ...
//
DA_NUM_TO_STR(WM_NULL                         ); // 0x0000
DA_NUM_TO_STR(WM_CREATE                       ); // 0x0001
DA_NUM_TO_STR(WM_DESTROY                      ); // 0x0002
DA_NUM_TO_STR(WM_MOVE                         ); // 0x0003
DA_NUM_TO_STR(WM_SIZE                         ); // 0x0005
DA_NUM_TO_STR(WM_ACTIVATE                     ); // 0x0006
DA_NUM_TO_STR(WM_SETFOCUS                     ); // 0x0007
DA_NUM_TO_STR(WM_KILLFOCUS                    ); // 0x0008
DA_NUM_TO_STR(WM_ENABLE                       ); // 0x000A
DA_NUM_TO_STR(WM_SETREDRAW                    ); // 0x000B
DA_NUM_TO_STR(WM_SETTEXT                      ); // 0x000C
DA_NUM_TO_STR(WM_GETTEXT                      ); // 0x000D
DA_NUM_TO_STR(WM_GETTEXTLENGTH                ); // 0x000E
DA_NUM_TO_STR(WM_PAINT                        ); // 0x000F
DA_NUM_TO_STR(WM_CLOSE                        ); // 0x0010
#ifndef _WIN32_WCE
DA_NUM_TO_STR(WM_QUERYENDSESSION              ); // 0x0011
DA_NUM_TO_STR(WM_QUERYOPEN                    ); // 0x0013
DA_NUM_TO_STR(WM_ENDSESSION                   ); // 0x0016
#endif
DA_NUM_TO_STR(WM_QUIT                         ); // 0x0012
DA_NUM_TO_STR(WM_ERASEBKGND                   ); // 0x0014
DA_NUM_TO_STR(WM_SYSCOLORCHANGE               ); // 0x0015
DA_NUM_TO_STR(WM_SHOWWINDOW                   ); // 0x0018
DA_NUM_TO_STR(WM_WININICHANGE                 ); // 0x001A
DA_NUM_TO_STR(WM_DEVMODECHANGE                ); // 0x001B
DA_NUM_TO_STR(WM_ACTIVATEAPP                  ); // 0x001C
DA_NUM_TO_STR(WM_FONTCHANGE                   ); // 0x001D
DA_NUM_TO_STR(WM_TIMECHANGE                   ); // 0x001E
DA_NUM_TO_STR(WM_CANCELMODE                   ); // 0x001F
DA_NUM_TO_STR(WM_SETCURSOR                    ); // 0x0020
DA_NUM_TO_STR(WM_MOUSEACTIVATE                ); // 0x0021
DA_NUM_TO_STR(WM_CHILDACTIVATE                ); // 0x0022
DA_NUM_TO_STR(WM_QUEUESYNC                    ); // 0x0023
DA_NUM_TO_STR(WM_GETMINMAXINFO                ); // 0x0024
DA_NUM_TO_STR(WM_PAINTICON                    ); // 0x0026
DA_NUM_TO_STR(WM_ICONERASEBKGND               ); // 0x0027
DA_NUM_TO_STR(WM_NEXTDLGCTL                   ); // 0x0028
DA_NUM_TO_STR(WM_SPOOLERSTATUS                ); // 0x002A
DA_NUM_TO_STR(WM_DRAWITEM                     ); // 0x002B
DA_NUM_TO_STR(WM_MEASUREITEM                  ); // 0x002C
DA_NUM_TO_STR(WM_DELETEITEM                   ); // 0x002D
DA_NUM_TO_STR(WM_VKEYTOITEM                   ); // 0x002E
DA_NUM_TO_STR(WM_CHARTOITEM                   ); // 0x002F
DA_NUM_TO_STR(WM_SETFONT                      ); // 0x0030
DA_NUM_TO_STR(WM_GETFONT                      ); // 0x0031
DA_NUM_TO_STR(WM_SETHOTKEY                    ); // 0x0032
DA_NUM_TO_STR(WM_GETHOTKEY                    ); // 0x0033
DA_NUM_TO_STR(WM_QUERYDRAGICON                ); // 0x0037
DA_NUM_TO_STR(WM_COMPAREITEM                  ); // 0x0039
#if(WINVER >= 0x0500)
#ifndef _WIN32_WCE
DA_NUM_TO_STR(WM_GETOBJECT                    ); // 0x003D
#endif
#endif
DA_NUM_TO_STR(WM_COMPACTING                   ); // 0x0041
DA_NUM_TO_STR(WM_COMMNOTIFY                   ); // 0x0044  // no longer supported
DA_NUM_TO_STR(WM_WINDOWPOSCHANGING            ); // 0x0046
DA_NUM_TO_STR(WM_WINDOWPOSCHANGED             ); // 0x0047
DA_NUM_TO_STR(WM_POWER                        ); // 0x0048
DA_NUM_TO_STR(WM_COPYDATA                     ); // 0x004A
DA_NUM_TO_STR(WM_CANCELJOURNAL                ); // 0x004B
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(WM_NOTIFY                       ); // 0x004E
DA_NUM_TO_STR(WM_INPUTLANGCHANGEREQUEST       ); // 0x0050
DA_NUM_TO_STR(WM_INPUTLANGCHANGE              ); // 0x0051
DA_NUM_TO_STR(WM_TCARD                        ); // 0x0052
DA_NUM_TO_STR(WM_HELP                         ); // 0x0053
DA_NUM_TO_STR(WM_USERCHANGED                  ); // 0x0054
DA_NUM_TO_STR(WM_NOTIFYFORMAT                 ); // 0x0055
DA_NUM_TO_STR(WM_CONTEXTMENU                  ); // 0x007B
DA_NUM_TO_STR(WM_STYLECHANGING                ); // 0x007C
DA_NUM_TO_STR(WM_STYLECHANGED                 ); // 0x007D
DA_NUM_TO_STR(WM_DISPLAYCHANGE                ); // 0x007E
DA_NUM_TO_STR(WM_GETICON                      ); // 0x007F
DA_NUM_TO_STR(WM_SETICON                      ); // 0x0080
#endif
DA_NUM_TO_STR(WM_NCCREATE                     ); // 0x0081
DA_NUM_TO_STR(WM_NCDESTROY                    ); // 0x0082
DA_NUM_TO_STR(WM_NCCALCSIZE                   ); // 0x0083
DA_NUM_TO_STR(WM_NCHITTEST                    ); // 0x0084
DA_NUM_TO_STR(WM_NCPAINT                      ); // 0x0085
DA_NUM_TO_STR(WM_NCACTIVATE                   ); // 0x0086
DA_NUM_TO_STR(WM_GETDLGCODE                   ); // 0x0087
#ifndef _WIN32_WCE
DA_NUM_TO_STR(WM_SYNCPAINT                    ); // 0x0088
#endif
DA_NUM_TO_STR(WM_NCMOUSEMOVE                  ); // 0x00A0
DA_NUM_TO_STR(WM_NCLBUTTONDOWN                ); // 0x00A1
DA_NUM_TO_STR(WM_NCLBUTTONUP                  ); // 0x00A2
DA_NUM_TO_STR(WM_NCLBUTTONDBLCLK              ); // 0x00A3
DA_NUM_TO_STR(WM_NCRBUTTONDOWN                ); // 0x00A4
DA_NUM_TO_STR(WM_NCRBUTTONUP                  ); // 0x00A5
DA_NUM_TO_STR(WM_NCRBUTTONDBLCLK              ); // 0x00A6
DA_NUM_TO_STR(WM_NCMBUTTONDOWN                ); // 0x00A7
DA_NUM_TO_STR(WM_NCMBUTTONUP                  ); // 0x00A8
DA_NUM_TO_STR(WM_NCMBUTTONDBLCLK              ); // 0x00A9
#if(_WIN32_WINNT >= 0x0500)
DA_NUM_TO_STR(WM_NCXBUTTONDOWN                ); // 0x00AB
DA_NUM_TO_STR(WM_NCXBUTTONUP                  ); // 0x00AC
DA_NUM_TO_STR(WM_NCXBUTTONDBLCLK              ); // 0x00AD
#endif 
#if(_WIN32_WINNT >= 0x0501)
DA_NUM_TO_STR(WM_INPUT_DEVICE_CHANGE          ); // 0x00FE
#endif 
#if(_WIN32_WINNT >= 0x0501)
DA_NUM_TO_STR(WM_INPUT                        ); // 0x00FF
#endif 

DA_NUM_TO_STR(WM_KEYDOWN                      ); // 0x0100
DA_NUM_TO_STR(WM_KEYUP                        ); // 0x0101
DA_NUM_TO_STR(WM_CHAR                         ); // 0x0102
DA_NUM_TO_STR(WM_DEADCHAR                     ); // 0x0103
DA_NUM_TO_STR(WM_SYSKEYDOWN                   ); // 0x0104
DA_NUM_TO_STR(WM_SYSKEYUP                     ); // 0x0105
DA_NUM_TO_STR(WM_SYSCHAR                      ); // 0x0106
DA_NUM_TO_STR(WM_SYSDEADCHAR                  ); // 0x0107
#if(_WIN32_WINNT >= 0x0501)
DA_NUM_TO_STR(WM_UNICHAR                      ); // 0x0109
#else
DA_NUM_TO_STR(WM_KEYLAST                      ); // 0x0108
#endif

#if(WINVER >= 0x0400)
DA_NUM_TO_STR(WM_IME_STARTCOMPOSITION         ); // 0x010D
DA_NUM_TO_STR(WM_IME_ENDCOMPOSITION           ); // 0x010E
DA_NUM_TO_STR(WM_IME_COMPOSITION              ); // 0x010F
#endif 
DA_NUM_TO_STR(WM_INITDIALOG                   ); // 0x0110
DA_NUM_TO_STR(WM_COMMAND                      ); // 0x0111
DA_NUM_TO_STR(WM_SYSCOMMAND                   ); // 0x0112
DA_NUM_TO_STR(WM_TIMER                        ); // 0x0113
DA_NUM_TO_STR(WM_HSCROLL                      ); // 0x0114
DA_NUM_TO_STR(WM_VSCROLL                      ); // 0x0115
DA_NUM_TO_STR(WM_INITMENU                     ); // 0x0116
DA_NUM_TO_STR(WM_INITMENUPOPUP                ); // 0x0117
DA_NUM_TO_STR(WM_MENUSELECT                   ); // 0x011F
DA_NUM_TO_STR(WM_MENUCHAR                     ); // 0x0120
DA_NUM_TO_STR(WM_ENTERIDLE                    ); // 0x0121
#if(WINVER >= 0x0500)
#ifndef _WIN32_WCE
DA_NUM_TO_STR(WM_MENURBUTTONUP                ); // 0x0122
DA_NUM_TO_STR(WM_MENUDRAG                     ); // 0x0123
DA_NUM_TO_STR(WM_MENUGETOBJECT                ); // 0x0124
DA_NUM_TO_STR(WM_UNINITMENUPOPUP              ); // 0x0125
DA_NUM_TO_STR(WM_MENUCOMMAND                  ); // 0x0126

#ifndef _WIN32_WCE
#if(_WIN32_WINNT >= 0x0500)
DA_NUM_TO_STR(WM_CHANGEUISTATE                ); // 0x0127
DA_NUM_TO_STR(WM_UPDATEUISTATE                ); // 0x0128
DA_NUM_TO_STR(WM_QUERYUISTATE                 ); // 0x0129

#endif
#endif
#endif
#endif
DA_NUM_TO_STR(WM_CTLCOLORMSGBOX               ); // 0x0132
DA_NUM_TO_STR(WM_CTLCOLOREDIT                 ); // 0x0133
DA_NUM_TO_STR(WM_CTLCOLORLISTBOX              ); // 0x0134
DA_NUM_TO_STR(WM_CTLCOLORBTN                  ); // 0x0135
DA_NUM_TO_STR(WM_CTLCOLORDLG                  ); // 0x0136
DA_NUM_TO_STR(WM_CTLCOLORSCROLLBAR            ); // 0x0137
DA_NUM_TO_STR(WM_CTLCOLORSTATIC               ); // 0x0138
DA_NUM_TO_STR(MN_GETHMENU                     ); // 0x01E1
DA_NUM_TO_STR(WM_MOUSEMOVE                    ); // 0x0200
DA_NUM_TO_STR(WM_LBUTTONDOWN                  ); // 0x0201
DA_NUM_TO_STR(WM_LBUTTONUP                    ); // 0x0202
DA_NUM_TO_STR(WM_LBUTTONDBLCLK                ); // 0x0203
DA_NUM_TO_STR(WM_RBUTTONDOWN                  ); // 0x0204
DA_NUM_TO_STR(WM_RBUTTONUP                    ); // 0x0205
DA_NUM_TO_STR(WM_RBUTTONDBLCLK                ); // 0x0206
DA_NUM_TO_STR(WM_MBUTTONDOWN                  ); // 0x0207
DA_NUM_TO_STR(WM_MBUTTONUP                    ); // 0x0208
DA_NUM_TO_STR(WM_MBUTTONDBLCLK                ); // 0x0209
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
DA_NUM_TO_STR(WM_MOUSEWHEEL                   ); // 0x020A
#endif
#if (_WIN32_WINNT >= 0x0500)
DA_NUM_TO_STR(WM_XBUTTONDOWN                  ); // 0x020B
DA_NUM_TO_STR(WM_XBUTTONUP                    ); // 0x020C
DA_NUM_TO_STR(WM_XBUTTONDBLCLK                ); // 0x020D
#endif
#if (_WIN32_WINNT >= 0x0600)
DA_NUM_TO_STR(WM_MOUSEHWHEEL                  ); // 0x020E
#endif
DA_NUM_TO_STR(WM_PARENTNOTIFY                 ); // 0x0210
DA_NUM_TO_STR(WM_ENTERMENULOOP                ); // 0x0211
DA_NUM_TO_STR(WM_EXITMENULOOP                 ); // 0x0212
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(WM_NEXTMENU                     ); // 0x0213
DA_NUM_TO_STR(WM_SIZING                       ); // 0x0214
DA_NUM_TO_STR(WM_CAPTURECHANGED               ); // 0x0215
DA_NUM_TO_STR(WM_MOVING                       ); // 0x0216
#endif // WINVER >= 0x0400
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(WM_POWERBROADCAST               ); // 0x0218
#endif // WINVER >= 0x0400
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(WM_DEVICECHANGE                 ); // 0x0219
#endif // WINVER >= 0x0400

DA_NUM_TO_STR(WM_MDICREATE                    ); // 0x0220
DA_NUM_TO_STR(WM_MDIDESTROY                   ); // 0x0221
DA_NUM_TO_STR(WM_MDIACTIVATE                  ); // 0x0222
DA_NUM_TO_STR(WM_MDIRESTORE                   ); // 0x0223
DA_NUM_TO_STR(WM_MDINEXT                      ); // 0x0224
DA_NUM_TO_STR(WM_MDIMAXIMIZE                  ); // 0x0225
DA_NUM_TO_STR(WM_MDITILE                      ); // 0x0226
DA_NUM_TO_STR(WM_MDICASCADE                   ); // 0x0227
DA_NUM_TO_STR(WM_MDIICONARRANGE               ); // 0x0228
DA_NUM_TO_STR(WM_MDIGETACTIVE                 ); // 0x0229
DA_NUM_TO_STR(WM_MDISETMENU                   ); // 0x0230
DA_NUM_TO_STR(WM_ENTERSIZEMOVE                ); // 0x0231
DA_NUM_TO_STR(WM_EXITSIZEMOVE                 ); // 0x0232
DA_NUM_TO_STR(WM_DROPFILES                    ); // 0x0233
DA_NUM_TO_STR(WM_MDIREFRESHMENU               ); // 0x0234
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(WM_IME_SETCONTEXT               ); // 0x0281
DA_NUM_TO_STR(WM_IME_NOTIFY                   ); // 0x0282
DA_NUM_TO_STR(WM_IME_CONTROL                  ); // 0x0283
DA_NUM_TO_STR(WM_IME_COMPOSITIONFULL          ); // 0x0284
DA_NUM_TO_STR(WM_IME_SELECT                   ); // 0x0285
DA_NUM_TO_STR(WM_IME_CHAR                     ); // 0x0286
#endif // WINVER >= 0x0400
#if(WINVER >= 0x0500)
DA_NUM_TO_STR(WM_IME_REQUEST                  ); // 0x0288
#endif // WINVER >= 0x0500
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(WM_IME_KEYDOWN                  ); // 0x0290
DA_NUM_TO_STR(WM_IME_KEYUP                    ); // 0x0291
#endif // WINVER >= 0x0400
#if((_WIN32_WINNT >= 0x0400) || (WINVER >= 0x0500))
DA_NUM_TO_STR(WM_MOUSEHOVER                   ); // 0x02A1
DA_NUM_TO_STR(WM_MOUSELEAVE                   ); // 0x02A3
#endif
#if(WINVER >= 0x0500)
DA_NUM_TO_STR(WM_NCMOUSEHOVER                 ); // 0x02A0
DA_NUM_TO_STR(WM_NCMOUSELEAVE                 ); // 0x02A2
#endif // WINVER >= 0x0500
#if(_WIN32_WINNT >= 0x0501)
DA_NUM_TO_STR(WM_WTSSESSION_CHANGE            ); // 0x02B1
DA_NUM_TO_STR(WM_TABLET_FIRST                 ); // 0x02c0
DA_NUM_TO_STR(WM_TABLET_LAST                  ); // 0x02df
#endif // _WIN32_WINNT >= 0x0501
DA_NUM_TO_STR(WM_CUT                          ); // 0x0300
DA_NUM_TO_STR(WM_COPY                         ); // 0x0301
DA_NUM_TO_STR(WM_PASTE                        ); // 0x0302
DA_NUM_TO_STR(WM_CLEAR                        ); // 0x0303
DA_NUM_TO_STR(WM_UNDO                         ); // 0x0304
DA_NUM_TO_STR(WM_RENDERFORMAT                 ); // 0x0305
DA_NUM_TO_STR(WM_RENDERALLFORMATS             ); // 0x0306
DA_NUM_TO_STR(WM_DESTROYCLIPBOARD             ); // 0x0307
DA_NUM_TO_STR(WM_DRAWCLIPBOARD                ); // 0x0308
DA_NUM_TO_STR(WM_PAINTCLIPBOARD               ); // 0x0309
DA_NUM_TO_STR(WM_VSCROLLCLIPBOARD             ); // 0x030A
DA_NUM_TO_STR(WM_SIZECLIPBOARD                ); // 0x030B
DA_NUM_TO_STR(WM_ASKCBFORMATNAME              ); // 0x030C
DA_NUM_TO_STR(WM_CHANGECBCHAIN                ); // 0x030D
DA_NUM_TO_STR(WM_HSCROLLCLIPBOARD             ); // 0x030E
DA_NUM_TO_STR(WM_QUERYNEWPALETTE              ); // 0x030F
DA_NUM_TO_STR(WM_PALETTEISCHANGING            ); // 0x0310
DA_NUM_TO_STR(WM_PALETTECHANGED               ); // 0x0311
DA_NUM_TO_STR(WM_HOTKEY                       ); // 0x0312
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(WM_PRINT                        ); // 0x0317
DA_NUM_TO_STR(WM_PRINTCLIENT                  ); // 0x0318
#endif // WINVER >= 0x0400
#if(_WIN32_WINNT >= 0x0500)
DA_NUM_TO_STR(WM_APPCOMMAND                   ); // 0x0319
#endif // _WIN32_WINNT >= 0x0500
#if(_WIN32_WINNT >= 0x0501)
DA_NUM_TO_STR(WM_THEMECHANGED                 ); // 0x031A
#endif // _WIN32_WINNT >= 0x0501
#if(_WIN32_WINNT >= 0x0501)
DA_NUM_TO_STR(WM_CLIPBOARDUPDATE              ); // 0x031D
#endif // _WIN32_WINNT >= 0x0501
#if(_WIN32_WINNT >= 0x0600)
DA_NUM_TO_STR(WM_DWMCOMPOSITIONCHANGED        ); // 0x031E
DA_NUM_TO_STR(WM_DWMNCRENDERINGCHANGED        ); // 0x031F
DA_NUM_TO_STR(WM_DWMCOLORIZATIONCOLORCHANGED  ); // 0x0320
DA_NUM_TO_STR(WM_DWMWINDOWMAXIMIZEDCHANGE     ); // 0x0321
#endif // _WIN32_WINNT >= 0x0600
#if(WINVER >= 0x0600)
DA_NUM_TO_STR(WM_GETTITLEBARINFOEX            ); // 0x033F
#endif // WINVER >= 0x0600
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(WM_HANDHELDFIRST                ); // 0x0358
DA_NUM_TO_STR(WM_HANDHELDLAST                 ); // 0x035F
DA_NUM_TO_STR(WM_AFXFIRST                     ); // 0x0360
DA_NUM_TO_STR(WM_AFXLAST                      ); // 0x037F
#endif // WINVER >= 0x0400
DA_NUM_TO_STR(WM_PENWINFIRST                  ); // 0x0380
DA_NUM_TO_STR(WM_PENWINLAST                   ); // 0x038F
DA_NUM_TO_STR(WM_USER                         ); // 0x0400
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(WM_APP                          ); // 0x8000
#endif // WINVER >= 0x0400
//////////////////////////////////////////////////////////////////////////
// Control Messages (General Control Reference)
// 
DA_NUM_TO_STR(CCM_DPISCALE          );
DA_NUM_TO_STR(CCM_GETUNICODEFORMAT  );
DA_NUM_TO_STR(CCM_GETVERSION        );
DA_NUM_TO_STR(CCM_SETUNICODEFORMAT  );
DA_NUM_TO_STR(CCM_SETVERSION        );
DA_NUM_TO_STR(CCM_SETWINDOWTHEME    );
//////////////////////////////////////////////////////////////////////////
// Edit messages
// 
DA_NUM_TO_STR(EM_GETSEL               ); // 0x00B0
DA_NUM_TO_STR(EM_SETSEL               ); // 0x00B1
DA_NUM_TO_STR(EM_GETRECT              ); // 0x00B2
DA_NUM_TO_STR(EM_SETRECT              ); // 0x00B3
DA_NUM_TO_STR(EM_SETRECTNP            ); // 0x00B4
DA_NUM_TO_STR(EM_SCROLL               ); // 0x00B5
DA_NUM_TO_STR(EM_LINESCROLL           ); // 0x00B6
DA_NUM_TO_STR(EM_SCROLLCARET          ); // 0x00B7
DA_NUM_TO_STR(EM_GETMODIFY            ); // 0x00B8
DA_NUM_TO_STR(EM_SETMODIFY            ); // 0x00B9
DA_NUM_TO_STR(EM_GETLINECOUNT         ); // 0x00BA
DA_NUM_TO_STR(EM_LINEINDEX            ); // 0x00BB
DA_NUM_TO_STR(EM_SETHANDLE            ); // 0x00BC
DA_NUM_TO_STR(EM_GETHANDLE            ); // 0x00BD
DA_NUM_TO_STR(EM_GETTHUMB             ); // 0x00BE
DA_NUM_TO_STR(EM_LINELENGTH           ); // 0x00C1
DA_NUM_TO_STR(EM_REPLACESEL           ); // 0x00C2
DA_NUM_TO_STR(EM_GETLINE              ); // 0x00C4
DA_NUM_TO_STR(EM_LIMITTEXT            ); // 0x00C5
DA_NUM_TO_STR(EM_CANUNDO              ); // 0x00C6
DA_NUM_TO_STR(EM_UNDO                 ); // 0x00C7
DA_NUM_TO_STR(EM_FMTLINES             ); // 0x00C8
DA_NUM_TO_STR(EM_LINEFROMCHAR         ); // 0x00C9
DA_NUM_TO_STR(EM_SETTABSTOPS          ); // 0x00CB
DA_NUM_TO_STR(EM_SETPASSWORDCHAR      ); // 0x00CC
DA_NUM_TO_STR(EM_EMPTYUNDOBUFFER      ); // 0x00CD
DA_NUM_TO_STR(EM_GETFIRSTVISIBLELINE  ); // 0x00CE
DA_NUM_TO_STR(EM_SETREADONLY          ); // 0x00CF
DA_NUM_TO_STR(EM_SETWORDBREAKPROC     ); // 0x00D0
DA_NUM_TO_STR(EM_GETWORDBREAKPROC     ); // 0x00D1
DA_NUM_TO_STR(EM_GETPASSWORDCHAR      ); // 0x00D2
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(EM_SETMARGINS           ); // 0x00D3
DA_NUM_TO_STR(EM_GETMARGINS           ); // 0x00D4
DA_NUM_TO_STR(EM_GETLIMITTEXT         ); // 0x00D5
DA_NUM_TO_STR(EM_POSFROMCHAR          ); // 0x00D6
DA_NUM_TO_STR(EM_CHARFROMPOS          ); // 0x00D7
#endif // WINVER >= 0x0400

#if(WINVER >= 0x0500)
DA_NUM_TO_STR(EM_SETIMESTATUS         ); // 0x00D8
DA_NUM_TO_STR(EM_GETIMESTATUS         ); // 0x00D9
#endif // WINVER >= 0x0500
//////////////////////////////////////////////////////////////////////////
// Button messages
// 
DA_NUM_TO_STR(BM_GETCHECK        ); // 0x00F0
DA_NUM_TO_STR(BM_SETCHECK        ); // 0x00F1
DA_NUM_TO_STR(BM_GETSTATE        ); // 0x00F2
DA_NUM_TO_STR(BM_SETSTATE        ); // 0x00F3
DA_NUM_TO_STR(BM_SETSTYLE        ); // 0x00F4
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(BM_CLICK           ); // 0x00F5
DA_NUM_TO_STR(BM_GETIMAGE        ); // 0x00F6
DA_NUM_TO_STR(BM_SETIMAGE        ); // 0x00F7
#endif // WINVER >= 0x0400
#if(WINVER >= 0x0600)
DA_NUM_TO_STR(BM_SETDONTCLICK    ); // 0x00F8
#endif // WINVER >= 0x0600
//////////////////////////////////////////////////////////////////////////
// Static messages
// 
DA_NUM_TO_STR(STM_SETICON         ); // 0x0170
DA_NUM_TO_STR(STM_GETICON         ); // 0x0171
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(STM_SETIMAGE        ); // 0x0172
DA_NUM_TO_STR(STM_GETIMAGE        ); // 0x0173
#endif // (WINVER >= 0x0400)
//////////////////////////////////////////////////////////////////////////
// ListBox messages
// 
DA_NUM_TO_STR(LB_ADDSTRING            ); // 0x0180
DA_NUM_TO_STR(LB_INSERTSTRING         ); // 0x0181
DA_NUM_TO_STR(LB_DELETESTRING         ); // 0x0182
DA_NUM_TO_STR(LB_SELITEMRANGEEX       ); // 0x0183
DA_NUM_TO_STR(LB_RESETCONTENT         ); // 0x0184
DA_NUM_TO_STR(LB_SETSEL               ); // 0x0185
DA_NUM_TO_STR(LB_SETCURSEL            ); // 0x0186
DA_NUM_TO_STR(LB_GETSEL               ); // 0x0187
DA_NUM_TO_STR(LB_GETCURSEL            ); // 0x0188
DA_NUM_TO_STR(LB_GETTEXT              ); // 0x0189
DA_NUM_TO_STR(LB_GETTEXTLEN           ); // 0x018A
DA_NUM_TO_STR(LB_GETCOUNT             ); // 0x018B
DA_NUM_TO_STR(LB_SELECTSTRING         ); // 0x018C
DA_NUM_TO_STR(LB_DIR                  ); // 0x018D
DA_NUM_TO_STR(LB_GETTOPINDEX          ); // 0x018E
DA_NUM_TO_STR(LB_FINDSTRING           ); // 0x018F
DA_NUM_TO_STR(LB_GETSELCOUNT          ); // 0x0190
DA_NUM_TO_STR(LB_GETSELITEMS          ); // 0x0191
DA_NUM_TO_STR(LB_SETTABSTOPS          ); // 0x0192
DA_NUM_TO_STR(LB_GETHORIZONTALEXTENT  ); // 0x0193
DA_NUM_TO_STR(LB_SETHORIZONTALEXTENT  ); // 0x0194
DA_NUM_TO_STR(LB_SETCOLUMNWIDTH       ); // 0x0195
DA_NUM_TO_STR(LB_ADDFILE              ); // 0x0196
DA_NUM_TO_STR(LB_SETTOPINDEX          ); // 0x0197
DA_NUM_TO_STR(LB_GETITEMRECT          ); // 0x0198
DA_NUM_TO_STR(LB_GETITEMDATA          ); // 0x0199
DA_NUM_TO_STR(LB_SETITEMDATA          ); // 0x019A
DA_NUM_TO_STR(LB_SELITEMRANGE         ); // 0x019B
DA_NUM_TO_STR(LB_SETANCHORINDEX       ); // 0x019C
DA_NUM_TO_STR(LB_GETANCHORINDEX       ); // 0x019D
DA_NUM_TO_STR(LB_SETCARETINDEX        ); // 0x019E
DA_NUM_TO_STR(LB_GETCARETINDEX        ); // 0x019F
DA_NUM_TO_STR(LB_SETITEMHEIGHT        ); // 0x01A0
DA_NUM_TO_STR(LB_GETITEMHEIGHT        ); // 0x01A1
DA_NUM_TO_STR(LB_FINDSTRINGEXACT      ); // 0x01A2
DA_NUM_TO_STR(LB_SETLOCALE            ); // 0x01A5
DA_NUM_TO_STR(LB_GETLOCALE            ); // 0x01A6
DA_NUM_TO_STR(LB_SETCOUNT             ); // 0x01A7
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(LB_INITSTORAGE          ); // 0x01A8
DA_NUM_TO_STR(LB_ITEMFROMPOINT        ); // 0x01A9
#endif // WINVER >= 0x0400
#if defined(_WIN32_WCE) && (_WIN32_WCE >= 0x0400)
DA_NUM_TO_STR(LB_MULTIPLEADDSTRING    ); // 0x01B1
#endif
#if(_WIN32_WINNT >= 0x0501)
DA_NUM_TO_STR(LB_GETLISTBOXINFO       ); // 0x01B2
#endif // _WIN32_WINNT >= 0x0501
//////////////////////////////////////////////////////////////////////////
// ComboBox messages
// 
DA_NUM_TO_STR(CB_GETEDITSEL               ); // 0x0140
DA_NUM_TO_STR(CB_LIMITTEXT                ); // 0x0141
DA_NUM_TO_STR(CB_SETEDITSEL               ); // 0x0142
DA_NUM_TO_STR(CB_ADDSTRING                ); // 0x0143
DA_NUM_TO_STR(CB_DELETESTRING             ); // 0x0144
DA_NUM_TO_STR(CB_DIR                      ); // 0x0145
DA_NUM_TO_STR(CB_GETCOUNT                 ); // 0x0146
DA_NUM_TO_STR(CB_GETCURSEL                ); // 0x0147
DA_NUM_TO_STR(CB_GETLBTEXT                ); // 0x0148
DA_NUM_TO_STR(CB_GETLBTEXTLEN             ); // 0x0149
DA_NUM_TO_STR(CB_INSERTSTRING             ); // 0x014A
DA_NUM_TO_STR(CB_RESETCONTENT             ); // 0x014B
DA_NUM_TO_STR(CB_FINDSTRING               ); // 0x014C
DA_NUM_TO_STR(CB_SELECTSTRING             ); // 0x014D
DA_NUM_TO_STR(CB_SETCURSEL                ); // 0x014E
DA_NUM_TO_STR(CB_SHOWDROPDOWN             ); // 0x014F
DA_NUM_TO_STR(CB_GETITEMDATA              ); // 0x0150
DA_NUM_TO_STR(CB_SETITEMDATA              ); // 0x0151
DA_NUM_TO_STR(CB_GETDROPPEDCONTROLRECT    ); // 0x0152
DA_NUM_TO_STR(CB_SETITEMHEIGHT            ); // 0x0153
DA_NUM_TO_STR(CB_GETITEMHEIGHT            ); // 0x0154
DA_NUM_TO_STR(CB_SETEXTENDEDUI            ); // 0x0155
DA_NUM_TO_STR(CB_GETEXTENDEDUI            ); // 0x0156
DA_NUM_TO_STR(CB_GETDROPPEDSTATE          ); // 0x0157
DA_NUM_TO_STR(CB_FINDSTRINGEXACT          ); // 0x0158
DA_NUM_TO_STR(CB_SETLOCALE                ); // 0x0159
DA_NUM_TO_STR(CB_GETLOCALE                ); // 0x015A
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(CB_GETTOPINDEX              ); // 0x015b
DA_NUM_TO_STR(CB_SETTOPINDEX              ); // 0x015c
DA_NUM_TO_STR(CB_GETHORIZONTALEXTENT      ); // 0x015d
DA_NUM_TO_STR(CB_SETHORIZONTALEXTENT      ); // 0x015e
DA_NUM_TO_STR(CB_GETDROPPEDWIDTH          ); // 0x015f
DA_NUM_TO_STR(CB_SETDROPPEDWIDTH          ); // 0x0160
DA_NUM_TO_STR(CB_INITSTORAGE              ); // 0x0161
#if defined(_WIN32_WCE) && (_WIN32_WCE >= 0x0400)
DA_NUM_TO_STR(CB_MULTIPLEADDSTRING        ); // 0x0163
#endif
#endif // WINVER >= 0x0400
#if(_WIN32_WINNT >= 0x0501)
DA_NUM_TO_STR(CB_GETCOMBOBOXINFO          ); // 0x0164
#endif // _WIN32_WINNT >= 0x0501
//////////////////////////////////////////////////////////////////////////
// ScrollBar messages
// 
DA_NUM_TO_STR(SBM_SETPOS                  ); // 0x00E0 // not in win3.1
DA_NUM_TO_STR(SBM_GETPOS                  ); // 0x00E1 // not in win3.1
DA_NUM_TO_STR(SBM_SETRANGE                ); // 0x00E2 // not in win3.1
DA_NUM_TO_STR(SBM_SETRANGEREDRAW          ); // 0x00E6 // not in win3.1
DA_NUM_TO_STR(SBM_GETRANGE                ); // 0x00E3 // not in win3.1
DA_NUM_TO_STR(SBM_ENABLE_ARROWS           ); // 0x00E4 // not in win3.1
#if(WINVER >= 0x0400)
DA_NUM_TO_STR(SBM_SETSCROLLINFO           ); // 0x00E9
DA_NUM_TO_STR(SBM_GETSCROLLINFO           ); // 0x00EA
#endif // WINVER >= 0x0400
#if(_WIN32_WINNT >= 0x0501)
DA_NUM_TO_STR(SBM_GETSCROLLBARINFO        ); // 0x00EB
#endif // _WIN32_WINNT >= 0x0501
//////////////////////////////////////////////////////////////////////////
// Tree View Messages
// 
DA_NUM_TO_STR(TVM_CREATEDRAGIMAGE           );
DA_NUM_TO_STR(TVM_DELETEITEM                );
DA_NUM_TO_STR(TVM_EDITLABEL                 );
DA_NUM_TO_STR(TVM_ENDEDITLABELNOW           );
DA_NUM_TO_STR(TVM_ENSUREVISIBLE             );
DA_NUM_TO_STR(TVM_EXPAND                    );
DA_NUM_TO_STR(TVM_GETBKCOLOR                );
DA_NUM_TO_STR(TVM_GETCOUNT                  );
DA_NUM_TO_STR(TVM_GETEDITCONTROL            );
DA_NUM_TO_STR(TVM_GETEXTENDEDSTYLE          );
DA_NUM_TO_STR(TVM_GETIMAGELIST              );
DA_NUM_TO_STR(TVM_GETINDENT                 );
DA_NUM_TO_STR(TVM_GETINSERTMARKCOLOR        );
DA_NUM_TO_STR(TVM_GETISEARCHSTRING          );
DA_NUM_TO_STR(TVM_GETITEM                   );
DA_NUM_TO_STR(TVM_GETITEMHEIGHT             );
DA_NUM_TO_STR(TVM_GETITEMPARTRECT           );
DA_NUM_TO_STR(TVM_GETITEMRECT               );
DA_NUM_TO_STR(TVM_GETITEMSTATE              );
DA_NUM_TO_STR(TVM_GETLINECOLOR              );
DA_NUM_TO_STR(TVM_GETNEXTITEM               );
DA_NUM_TO_STR(TVM_GETSCROLLTIME             );
DA_NUM_TO_STR(TVM_GETSELECTEDCOUNT          );
DA_NUM_TO_STR(TVM_GETTEXTCOLOR              );
DA_NUM_TO_STR(TVM_GETTOOLTIPS               );
//DA_NUM_TO_STR(TVM_GETUNICODEFORMAT          ); // CCM_GETUNICODEFORMAT
DA_NUM_TO_STR(TVM_GETVISIBLECOUNT           );
DA_NUM_TO_STR(TVM_HITTEST                   );
DA_NUM_TO_STR(TVM_INSERTITEM                );
DA_NUM_TO_STR(TVM_MAPACCIDTOHTREEITEM       );
DA_NUM_TO_STR(TVM_MAPHTREEITEMTOACCID       );
DA_NUM_TO_STR(TVM_SELECTITEM                );
DA_NUM_TO_STR(TVM_SETAUTOSCROLLINFO         );
DA_NUM_TO_STR(TVM_SETBKCOLOR                );
DA_NUM_TO_STR(TVM_SETBORDER                 );
DA_NUM_TO_STR(TVM_SETEXTENDEDSTYLE          );
DA_NUM_TO_STR(TVM_SETHOT                    );
DA_NUM_TO_STR(TVM_SETIMAGELIST              );
DA_NUM_TO_STR(TVM_SETINDENT                 );
DA_NUM_TO_STR(TVM_SETINSERTMARK             );
DA_NUM_TO_STR(TVM_SETINSERTMARKCOLOR        );
DA_NUM_TO_STR(TVM_SETITEM                   );
DA_NUM_TO_STR(TVM_SETITEMHEIGHT             );
DA_NUM_TO_STR(TVM_SETLINECOLOR              );
DA_NUM_TO_STR(TVM_SETSCROLLTIME             );
DA_NUM_TO_STR(TVM_SETTEXTCOLOR              );
DA_NUM_TO_STR(TVM_SETTOOLTIPS               );
//DA_NUM_TO_STR(TVM_SETUNICODEFORMAT          ); // CCM_SETUNICODEFORMAT
DA_NUM_TO_STR(TVM_SHOWINFOTIP               );
DA_NUM_TO_STR(TVM_SORTCHILDREN              );
DA_NUM_TO_STR(TVM_SORTCHILDRENCB            );
////////////////////////////////////////////////////////////////////////////
//// Tooltip Control Messages
//// 
////   TTM_ACTIVATE = WM_USER + 1
//// 
//    DA_NUM_TO_STR(TTM_ACTIVATE                  );
//    DA_NUM_TO_STR(TTM_ADDTOOL                   );
//    DA_NUM_TO_STR(TTM_ADJUSTRECT                );
//    DA_NUM_TO_STR(TTM_DELTOOL                   );
//    DA_NUM_TO_STR(TTM_ENUMTOOLS                 );
//    DA_NUM_TO_STR(TTM_GETBUBBLESIZE             );
//    DA_NUM_TO_STR(TTM_GETCURRENTTOOL            );
//    DA_NUM_TO_STR(TTM_GETDELAYTIME              );
//    DA_NUM_TO_STR(TTM_GETMARGIN                 );
//    DA_NUM_TO_STR(TTM_GETMAXTIPWIDTH            );
//    DA_NUM_TO_STR(TTM_GETTEXT                   );
//    DA_NUM_TO_STR(TTM_GETTIPBKCOLOR             );
//    DA_NUM_TO_STR(TTM_GETTIPTEXTCOLOR           );
//    DA_NUM_TO_STR(TTM_GETTITLE                  );
//    DA_NUM_TO_STR(TTM_GETTOOLCOUNT              );
//    DA_NUM_TO_STR(TTM_GETTOOLINFO               );
//    DA_NUM_TO_STR(TTM_HITTEST                   );
//    DA_NUM_TO_STR(TTM_NEWTOOLRECT               );
//    DA_NUM_TO_STR(TTM_POP                       );
//    DA_NUM_TO_STR(TTM_POPUP                     );
//    DA_NUM_TO_STR(TTM_RELAYEVENT                );
//    DA_NUM_TO_STR(TTM_SETDELAYTIME              );
//    DA_NUM_TO_STR(TTM_SETMARGIN                 );
//    DA_NUM_TO_STR(TTM_SETMAXTIPWIDTH            );
//    DA_NUM_TO_STR(TTM_SETTIPBKCOLOR             );
//    DA_NUM_TO_STR(TTM_SETTIPTEXTCOLOR           );
//    DA_NUM_TO_STR(TTM_SETTITLE                  );
//    DA_NUM_TO_STR(TTM_SETTOOLINFO               );
//    DA_NUM_TO_STR(TTM_SETWINDOWTHEME            );
//    DA_NUM_TO_STR(TTM_TRACKACTIVATE             );
//    DA_NUM_TO_STR(TTM_TRACKPOSITION             );
//    DA_NUM_TO_STR(TTM_UPDATE                    );
//    DA_NUM_TO_STR(TTM_UPDATETIPTEXT             );
//    DA_NUM_TO_STR(TTM_WINDOWFROMPOINT           );
//////////////////////////////////////////////////////////////////////////
// List View Messages
// 
DA_NUM_TO_STR(LVM_APPROXIMATEVIEWRECT       );
DA_NUM_TO_STR(LVM_ARRANGE                   );
DA_NUM_TO_STR(LVM_CANCELEDITLABEL           );
DA_NUM_TO_STR(LVM_CREATEDRAGIMAGE           );
DA_NUM_TO_STR(LVM_DELETEALLITEMS            );
DA_NUM_TO_STR(LVM_DELETECOLUMN              );
DA_NUM_TO_STR(LVM_DELETEITEM                );
DA_NUM_TO_STR(LVM_EDITLABEL                 );
DA_NUM_TO_STR(LVM_ENABLEGROUPVIEW           );
DA_NUM_TO_STR(LVM_ENSUREVISIBLE             );
DA_NUM_TO_STR(LVM_FINDITEM                  );
DA_NUM_TO_STR(LVM_GETBKCOLOR                );
DA_NUM_TO_STR(LVM_GETBKIMAGE                );
DA_NUM_TO_STR(LVM_GETCALLBACKMASK           );
DA_NUM_TO_STR(LVM_GETCOLUMN                 );
DA_NUM_TO_STR(LVM_GETCOLUMNORDERARRAY       );
DA_NUM_TO_STR(LVM_GETCOLUMNWIDTH            );
DA_NUM_TO_STR(LVM_GETCOUNTPERPAGE           );
DA_NUM_TO_STR(LVM_GETEDITCONTROL            );
DA_NUM_TO_STR(LVM_GETEMPTYTEXT              );
DA_NUM_TO_STR(LVM_GETEXTENDEDLISTVIEWSTYLE  );
DA_NUM_TO_STR(LVM_GETFOCUSEDGROUP           );
DA_NUM_TO_STR(LVM_GETFOOTERINFO             );
DA_NUM_TO_STR(LVM_GETFOOTERITEM             );
DA_NUM_TO_STR(LVM_GETFOOTERITEMRECT         );
DA_NUM_TO_STR(LVM_GETFOOTERRECT             );
DA_NUM_TO_STR(LVM_GETGROUPCOUNT             );
DA_NUM_TO_STR(LVM_GETGROUPINFO              );
DA_NUM_TO_STR(LVM_GETGROUPINFOBYINDEX       );
DA_NUM_TO_STR(LVM_GETGROUPMETRICS           );
DA_NUM_TO_STR(LVM_GETGROUPRECT              );
DA_NUM_TO_STR(LVM_GETGROUPSTATE             );
DA_NUM_TO_STR(LVM_GETHEADER                 );
DA_NUM_TO_STR(LVM_GETHOTCURSOR              );
DA_NUM_TO_STR(LVM_GETHOTITEM                );
DA_NUM_TO_STR(LVM_GETHOVERTIME              );
DA_NUM_TO_STR(LVM_GETIMAGELIST              );
DA_NUM_TO_STR(LVM_GETINSERTMARK             );
DA_NUM_TO_STR(LVM_GETINSERTMARKCOLOR        );
DA_NUM_TO_STR(LVM_GETINSERTMARKRECT         );
DA_NUM_TO_STR(LVM_GETISEARCHSTRING          );
DA_NUM_TO_STR(LVM_GETITEM                   );
DA_NUM_TO_STR(LVM_GETITEMCOUNT              );
DA_NUM_TO_STR(LVM_GETITEMINDEXRECT          );
DA_NUM_TO_STR(LVM_GETITEMPOSITION           );
DA_NUM_TO_STR(LVM_GETITEMRECT               );
DA_NUM_TO_STR(LVM_GETITEMSPACING            );
DA_NUM_TO_STR(LVM_GETITEMSTATE              );
DA_NUM_TO_STR(LVM_GETITEMTEXT               );
DA_NUM_TO_STR(LVM_GETNEXTITEM               );
DA_NUM_TO_STR(LVM_GETNEXTITEMINDEX          );
DA_NUM_TO_STR(LVM_GETNUMBEROFWORKAREAS      );
DA_NUM_TO_STR(LVM_GETORIGIN                 );
DA_NUM_TO_STR(LVM_GETOUTLINECOLOR           );
DA_NUM_TO_STR(LVM_GETSELECTEDCOLUMN         );
DA_NUM_TO_STR(LVM_GETSELECTEDCOUNT          );
DA_NUM_TO_STR(LVM_GETSELECTIONMARK          );
DA_NUM_TO_STR(LVM_GETSTRINGWIDTH            );
DA_NUM_TO_STR(LVM_GETSUBITEMRECT            );
DA_NUM_TO_STR(LVM_GETTEXTBKCOLOR            );
DA_NUM_TO_STR(LVM_GETTEXTCOLOR              );
DA_NUM_TO_STR(LVM_GETTILEINFO               );
DA_NUM_TO_STR(LVM_GETTILEVIEWINFO           );
DA_NUM_TO_STR(LVM_GETTOOLTIPS               );
DA_NUM_TO_STR(LVM_GETTOPINDEX               );
//DA_NUM_TO_STR(LVM_GETUNICODEFORMAT          ); // CCM_GETUNICODEFORMAT
DA_NUM_TO_STR(LVM_GETVIEW                   );
DA_NUM_TO_STR(LVM_GETVIEWRECT               );
DA_NUM_TO_STR(LVM_GETWORKAREAS              );
DA_NUM_TO_STR(LVM_HASGROUP                  );
DA_NUM_TO_STR(LVM_HITTEST                   );
DA_NUM_TO_STR(LVM_INSERTCOLUMN              );
DA_NUM_TO_STR(LVM_INSERTGROUP               );
DA_NUM_TO_STR(LVM_INSERTGROUPSORTED         );
DA_NUM_TO_STR(LVM_INSERTITEM                );
DA_NUM_TO_STR(LVM_INSERTMARKHITTEST         );
DA_NUM_TO_STR(LVM_ISGROUPVIEWENABLED        );
DA_NUM_TO_STR(LVM_ISITEMVISIBLE             );
DA_NUM_TO_STR(LVM_MAPIDTOINDEX              );
DA_NUM_TO_STR(LVM_MAPINDEXTOID              );
DA_NUM_TO_STR(LVM_MOVEGROUP                 );
DA_NUM_TO_STR(LVM_MOVEITEMTOGROUP           );
DA_NUM_TO_STR(LVM_REDRAWITEMS               );
DA_NUM_TO_STR(LVM_REMOVEALLGROUPS           );
DA_NUM_TO_STR(LVM_REMOVEGROUP               );
DA_NUM_TO_STR(LVM_SCROLL                    );
DA_NUM_TO_STR(LVM_SETBKCOLOR                );
DA_NUM_TO_STR(LVM_SETBKIMAGE                );
DA_NUM_TO_STR(LVM_SETCALLBACKMASK           );
DA_NUM_TO_STR(LVM_SETCOLUMN                 );
DA_NUM_TO_STR(LVM_SETCOLUMNORDERARRAY       );
DA_NUM_TO_STR(LVM_SETCOLUMNWIDTH            );
DA_NUM_TO_STR(LVM_SETEXTENDEDLISTVIEWSTYLE  );
DA_NUM_TO_STR(LVM_SETGROUPINFO              );
DA_NUM_TO_STR(LVM_SETGROUPMETRICS           );
DA_NUM_TO_STR(LVM_SETHOTCURSOR              );
DA_NUM_TO_STR(LVM_SETHOTITEM                );
DA_NUM_TO_STR(LVM_SETHOVERTIME              );
DA_NUM_TO_STR(LVM_SETICONSPACING            );
DA_NUM_TO_STR(LVM_SETIMAGELIST              );
DA_NUM_TO_STR(LVM_SETINFOTIP                );
DA_NUM_TO_STR(LVM_SETINSERTMARK             );
DA_NUM_TO_STR(LVM_SETINSERTMARKCOLOR        );
DA_NUM_TO_STR(LVM_SETITEM                   );
DA_NUM_TO_STR(LVM_SETITEMCOUNT              );
DA_NUM_TO_STR(LVM_SETITEMINDEXSTATE         );
DA_NUM_TO_STR(LVM_SETITEMPOSITION           );
DA_NUM_TO_STR(LVM_SETITEMPOSITION32         );
DA_NUM_TO_STR(LVM_SETITEMSTATE              );
DA_NUM_TO_STR(LVM_SETITEMTEXT               );
DA_NUM_TO_STR(LVM_SETOUTLINECOLOR           );
DA_NUM_TO_STR(LVM_SETSELECTEDCOLUMN         );
DA_NUM_TO_STR(LVM_SETSELECTIONMARK          );
DA_NUM_TO_STR(LVM_SETTEXTBKCOLOR            );
DA_NUM_TO_STR(LVM_SETTEXTCOLOR              );
DA_NUM_TO_STR(LVM_SETTILEINFO               );
DA_NUM_TO_STR(LVM_SETTILEVIEWINFO           );
//DA_NUM_TO_STR(LVM_SETTILEWIDTH              ); // not supported
DA_NUM_TO_STR(LVM_SETTOOLTIPS               );
//DA_NUM_TO_STR(LVM_SETUNICODEFORMAT          ); // CCM_SETUNICODEFORMAT
DA_NUM_TO_STR(LVM_SETVIEW                   );
DA_NUM_TO_STR(LVM_SETWORKAREAS              );
DA_NUM_TO_STR(LVM_SORTGROUPS                );
DA_NUM_TO_STR(LVM_SORTITEMS                 );
DA_NUM_TO_STR(LVM_SORTITEMSEX               );
DA_NUM_TO_STR(LVM_SUBITEMHITTEST            );
DA_NUM_TO_STR(LVM_UPDATE                    );

DA_NUM_TO_STR(OCM_COMMAND          );

#ifdef _WIN32
DA_NUM_TO_STR(OCM_CTLCOLORBTN      );
DA_NUM_TO_STR(OCM_CTLCOLOREDIT     );
DA_NUM_TO_STR(OCM_CTLCOLORDLG      );
DA_NUM_TO_STR(OCM_CTLCOLORLISTBOX  );
DA_NUM_TO_STR(OCM_CTLCOLORMSGBOX   );
DA_NUM_TO_STR(OCM_CTLCOLORSCROLLBAR);
DA_NUM_TO_STR(OCM_CTLCOLORSTATIC   );
#else
DA_NUM_TO_STR(OCM_CTLCOLOR         );
#endif

DA_NUM_TO_STR(OCM_DRAWITEM         );
DA_NUM_TO_STR(OCM_MEASUREITEM      );
DA_NUM_TO_STR(OCM_DELETEITEM       );
DA_NUM_TO_STR(OCM_VKEYTOITEM       );
DA_NUM_TO_STR(OCM_CHARTOITEM       );
DA_NUM_TO_STR(OCM_COMPAREITEM      );
DA_NUM_TO_STR(OCM_HSCROLL          );
DA_NUM_TO_STR(OCM_VSCROLL          );
DA_NUM_TO_STR(OCM_PARENTNOTIFY     );

#if (WINVER >= 0x0400)
DA_NUM_TO_STR(OCM_NOTIFY           );
#endif

DA_NUM_TO_STR(WM_UAHDESTROYWINDOW   );
DA_NUM_TO_STR(WM_UAHDRAWMENU        );
DA_NUM_TO_STR(WM_UAHDRAWMENUITEM    );
DA_NUM_TO_STR(WM_UAHINITMENU        );
DA_NUM_TO_STR(WM_UAHMEASUREMENUITEM );
DA_NUM_TO_STR(WM_UAHNCPAINTMENUPOPUP);
