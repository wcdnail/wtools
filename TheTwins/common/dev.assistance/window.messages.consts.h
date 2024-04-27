//////////////////////////////////////////////////////////////////////////
// Window messages ...
//
        _ON_CONST(WM_NULL                         ); // 0x0000
        _ON_CONST(WM_CREATE                       ); // 0x0001
        _ON_CONST(WM_DESTROY                      ); // 0x0002
        _ON_CONST(WM_MOVE                         ); // 0x0003
        _ON_CONST(WM_SIZE                         ); // 0x0005

        _ON_CONST(WM_ACTIVATE                     ); // 0x0006

        _ON_CONST(WM_SETFOCUS                     ); // 0x0007
        _ON_CONST(WM_KILLFOCUS                    ); // 0x0008
        _ON_CONST(WM_ENABLE                       ); // 0x000A
        _ON_CONST(WM_SETREDRAW                    ); // 0x000B
        _ON_CONST(WM_SETTEXT                      ); // 0x000C
        _ON_CONST(WM_GETTEXT                      ); // 0x000D
        _ON_CONST(WM_GETTEXTLENGTH                ); // 0x000E
        _ON_CONST(WM_PAINT                        ); // 0x000F
        _ON_CONST(WM_CLOSE                        ); // 0x0010
#ifndef _WIN32_WCE
        _ON_CONST(WM_QUERYENDSESSION              ); // 0x0011
        _ON_CONST(WM_QUERYOPEN                    ); // 0x0013
        _ON_CONST(WM_ENDSESSION                   ); // 0x0016
#endif
        _ON_CONST(WM_QUIT                         ); // 0x0012
        _ON_CONST(WM_ERASEBKGND                   ); // 0x0014
        _ON_CONST(WM_SYSCOLORCHANGE               ); // 0x0015
        _ON_CONST(WM_SHOWWINDOW                   ); // 0x0018
        _ON_CONST(WM_WININICHANGE                 ); // 0x001A
        _ON_CONST(WM_DEVMODECHANGE                ); // 0x001B
        _ON_CONST(WM_ACTIVATEAPP                  ); // 0x001C
        _ON_CONST(WM_FONTCHANGE                   ); // 0x001D
        _ON_CONST(WM_TIMECHANGE                   ); // 0x001E
        _ON_CONST(WM_CANCELMODE                   ); // 0x001F
        _ON_CONST(WM_SETCURSOR                    ); // 0x0020
        _ON_CONST(WM_MOUSEACTIVATE                ); // 0x0021
        _ON_CONST(WM_CHILDACTIVATE                ); // 0x0022
        _ON_CONST(WM_QUEUESYNC                    ); // 0x0023
        _ON_CONST(WM_GETMINMAXINFO                ); // 0x0024
        _ON_CONST(WM_PAINTICON                    ); // 0x0026
        _ON_CONST(WM_ICONERASEBKGND               ); // 0x0027
        _ON_CONST(WM_NEXTDLGCTL                   ); // 0x0028
        _ON_CONST(WM_SPOOLERSTATUS                ); // 0x002A
        _ON_CONST(WM_DRAWITEM                     ); // 0x002B
        _ON_CONST(WM_MEASUREITEM                  ); // 0x002C
        _ON_CONST(WM_DELETEITEM                   ); // 0x002D
        _ON_CONST(WM_VKEYTOITEM                   ); // 0x002E
        _ON_CONST(WM_CHARTOITEM                   ); // 0x002F
        _ON_CONST(WM_SETFONT                      ); // 0x0030
        _ON_CONST(WM_GETFONT                      ); // 0x0031
        _ON_CONST(WM_SETHOTKEY                    ); // 0x0032
        _ON_CONST(WM_GETHOTKEY                    ); // 0x0033
        _ON_CONST(WM_QUERYDRAGICON                ); // 0x0037
        _ON_CONST(WM_COMPAREITEM                  ); // 0x0039
#if(WINVER >= 0x0500)
#ifndef _WIN32_WCE
        _ON_CONST(WM_GETOBJECT                    ); // 0x003D
#endif
#endif
        _ON_CONST(WM_COMPACTING                   ); // 0x0041
        _ON_CONST(WM_COMMNOTIFY                   ); // 0x0044  // no longer supported
        _ON_CONST(WM_WINDOWPOSCHANGING            ); // 0x0046
        _ON_CONST(WM_WINDOWPOSCHANGED             ); // 0x0047

        _ON_CONST(WM_POWER                        ); // 0x0048

        _ON_CONST(WM_COPYDATA                     ); // 0x004A
        _ON_CONST(WM_CANCELJOURNAL                ); // 0x004B

#if(WINVER >= 0x0400)
        _ON_CONST(WM_NOTIFY                       ); // 0x004E
        _ON_CONST(WM_INPUTLANGCHANGEREQUEST       ); // 0x0050
        _ON_CONST(WM_INPUTLANGCHANGE              ); // 0x0051
        _ON_CONST(WM_TCARD                        ); // 0x0052
        _ON_CONST(WM_HELP                         ); // 0x0053
        _ON_CONST(WM_USERCHANGED                  ); // 0x0054
        _ON_CONST(WM_NOTIFYFORMAT                 ); // 0x0055

        _ON_CONST(WM_CONTEXTMENU                  ); // 0x007B
        _ON_CONST(WM_STYLECHANGING                ); // 0x007C
        _ON_CONST(WM_STYLECHANGED                 ); // 0x007D
        _ON_CONST(WM_DISPLAYCHANGE                ); // 0x007E
        _ON_CONST(WM_GETICON                      ); // 0x007F
        _ON_CONST(WM_SETICON                      ); // 0x0080
#endif 

        _ON_CONST(WM_NCCREATE                     ); // 0x0081
        _ON_CONST(WM_NCDESTROY                    ); // 0x0082
        _ON_CONST(WM_NCCALCSIZE                   ); // 0x0083
        _ON_CONST(WM_NCHITTEST                    ); // 0x0084
        _ON_CONST(WM_NCPAINT                      ); // 0x0085
        _ON_CONST(WM_NCACTIVATE                   ); // 0x0086
        _ON_CONST(WM_GETDLGCODE                   ); // 0x0087
#ifndef _WIN32_WCE
        _ON_CONST(WM_SYNCPAINT                    ); // 0x0088
#endif
        _ON_CONST(WM_NCMOUSEMOVE                  ); // 0x00A0
        _ON_CONST(WM_NCLBUTTONDOWN                ); // 0x00A1
        _ON_CONST(WM_NCLBUTTONUP                  ); // 0x00A2
        _ON_CONST(WM_NCLBUTTONDBLCLK              ); // 0x00A3
        _ON_CONST(WM_NCRBUTTONDOWN                ); // 0x00A4
        _ON_CONST(WM_NCRBUTTONUP                  ); // 0x00A5
        _ON_CONST(WM_NCRBUTTONDBLCLK              ); // 0x00A6
        _ON_CONST(WM_NCMBUTTONDOWN                ); // 0x00A7
        _ON_CONST(WM_NCMBUTTONUP                  ); // 0x00A8
        _ON_CONST(WM_NCMBUTTONDBLCLK              ); // 0x00A9

#if(_WIN32_WINNT >= 0x0500)
        _ON_CONST(WM_NCXBUTTONDOWN                ); // 0x00AB
        _ON_CONST(WM_NCXBUTTONUP                  ); // 0x00AC
        _ON_CONST(WM_NCXBUTTONDBLCLK              ); // 0x00AD
#endif 

#if(_WIN32_WINNT >= 0x0501)
        _ON_CONST(WM_INPUT_DEVICE_CHANGE          ); // 0x00FE
#endif 
#if(_WIN32_WINNT >= 0x0501)
        _ON_CONST(WM_INPUT                        ); // 0x00FF
#endif 

        _ON_CONST(WM_KEYDOWN                      ); // 0x0100
        _ON_CONST(WM_KEYUP                        ); // 0x0101
        _ON_CONST(WM_CHAR                         ); // 0x0102
        _ON_CONST(WM_DEADCHAR                     ); // 0x0103
        _ON_CONST(WM_SYSKEYDOWN                   ); // 0x0104
        _ON_CONST(WM_SYSKEYUP                     ); // 0x0105
        _ON_CONST(WM_SYSCHAR                      ); // 0x0106
        _ON_CONST(WM_SYSDEADCHAR                  ); // 0x0107
#if(_WIN32_WINNT >= 0x0501)
        _ON_CONST(WM_UNICHAR                      ); // 0x0109
#else
        _ON_CONST(WM_KEYLAST                      ); // 0x0108
#endif

#if(WINVER >= 0x0400)
        _ON_CONST(WM_IME_STARTCOMPOSITION         ); // 0x010D
        _ON_CONST(WM_IME_ENDCOMPOSITION           ); // 0x010E
        _ON_CONST(WM_IME_COMPOSITION              ); // 0x010F
#endif 
        _ON_CONST(WM_INITDIALOG                   ); // 0x0110
        _ON_CONST(WM_COMMAND                      ); // 0x0111
        _ON_CONST(WM_SYSCOMMAND                   ); // 0x0112
        _ON_CONST(WM_TIMER                        ); // 0x0113
        _ON_CONST(WM_HSCROLL                      ); // 0x0114
        _ON_CONST(WM_VSCROLL                      ); // 0x0115
        _ON_CONST(WM_INITMENU                     ); // 0x0116
        _ON_CONST(WM_INITMENUPOPUP                ); // 0x0117
        _ON_CONST(WM_MENUSELECT                   ); // 0x011F
        _ON_CONST(WM_MENUCHAR                     ); // 0x0120
        _ON_CONST(WM_ENTERIDLE                    ); // 0x0121
#if(WINVER >= 0x0500)
#ifndef _WIN32_WCE
        _ON_CONST(WM_MENURBUTTONUP                ); // 0x0122
        _ON_CONST(WM_MENUDRAG                     ); // 0x0123
        _ON_CONST(WM_MENUGETOBJECT                ); // 0x0124
        _ON_CONST(WM_UNINITMENUPOPUP              ); // 0x0125
        _ON_CONST(WM_MENUCOMMAND                  ); // 0x0126

#ifndef _WIN32_WCE
#if(_WIN32_WINNT >= 0x0500)
        _ON_CONST(WM_CHANGEUISTATE                ); // 0x0127
        _ON_CONST(WM_UPDATEUISTATE                ); // 0x0128
        _ON_CONST(WM_QUERYUISTATE                 ); // 0x0129

#endif
#endif
#endif
#endif
        _ON_CONST(WM_CTLCOLORMSGBOX               ); // 0x0132
        _ON_CONST(WM_CTLCOLOREDIT                 ); // 0x0133
        _ON_CONST(WM_CTLCOLORLISTBOX              ); // 0x0134
        _ON_CONST(WM_CTLCOLORBTN                  ); // 0x0135
        _ON_CONST(WM_CTLCOLORDLG                  ); // 0x0136
        _ON_CONST(WM_CTLCOLORSCROLLBAR            ); // 0x0137
        _ON_CONST(WM_CTLCOLORSTATIC               ); // 0x0138
        _ON_CONST(MN_GETHMENU                     ); // 0x01E1

        _ON_CONST(WM_MOUSEMOVE                    ); // 0x0200
        _ON_CONST(WM_LBUTTONDOWN                  ); // 0x0201
        _ON_CONST(WM_LBUTTONUP                    ); // 0x0202
        _ON_CONST(WM_LBUTTONDBLCLK                ); // 0x0203
        _ON_CONST(WM_RBUTTONDOWN                  ); // 0x0204
        _ON_CONST(WM_RBUTTONUP                    ); // 0x0205
        _ON_CONST(WM_RBUTTONDBLCLK                ); // 0x0206
        _ON_CONST(WM_MBUTTONDOWN                  ); // 0x0207
        _ON_CONST(WM_MBUTTONUP                    ); // 0x0208
        _ON_CONST(WM_MBUTTONDBLCLK                ); // 0x0209
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
        _ON_CONST(WM_MOUSEWHEEL                   ); // 0x020A
#endif
#if (_WIN32_WINNT >= 0x0500)
        _ON_CONST(WM_XBUTTONDOWN                  ); // 0x020B
        _ON_CONST(WM_XBUTTONUP                    ); // 0x020C
        _ON_CONST(WM_XBUTTONDBLCLK                ); // 0x020D
#endif
#if (_WIN32_WINNT >= 0x0600)
        _ON_CONST(WM_MOUSEHWHEEL                  ); // 0x020E
#endif

        _ON_CONST(WM_PARENTNOTIFY                 ); // 0x0210
        _ON_CONST(WM_ENTERMENULOOP                ); // 0x0211
        _ON_CONST(WM_EXITMENULOOP                 ); // 0x0212

#if(WINVER >= 0x0400)
        _ON_CONST(WM_NEXTMENU                     ); // 0x0213
        _ON_CONST(WM_SIZING                       ); // 0x0214
        _ON_CONST(WM_CAPTURECHANGED               ); // 0x0215
        _ON_CONST(WM_MOVING                       ); // 0x0216
#endif // WINVER >= 0x0400

#if(WINVER >= 0x0400)
        _ON_CONST(WM_POWERBROADCAST               ); // 0x0218
#endif // WINVER >= 0x0400

#if(WINVER >= 0x0400)
        _ON_CONST(WM_DEVICECHANGE                 ); // 0x0219
#endif // WINVER >= 0x0400

        _ON_CONST(WM_MDICREATE                    ); // 0x0220
        _ON_CONST(WM_MDIDESTROY                   ); // 0x0221
        _ON_CONST(WM_MDIACTIVATE                  ); // 0x0222
        _ON_CONST(WM_MDIRESTORE                   ); // 0x0223
        _ON_CONST(WM_MDINEXT                      ); // 0x0224
        _ON_CONST(WM_MDIMAXIMIZE                  ); // 0x0225
        _ON_CONST(WM_MDITILE                      ); // 0x0226
        _ON_CONST(WM_MDICASCADE                   ); // 0x0227
        _ON_CONST(WM_MDIICONARRANGE               ); // 0x0228
        _ON_CONST(WM_MDIGETACTIVE                 ); // 0x0229


        _ON_CONST(WM_MDISETMENU                   ); // 0x0230
        _ON_CONST(WM_ENTERSIZEMOVE                ); // 0x0231
        _ON_CONST(WM_EXITSIZEMOVE                 ); // 0x0232
        _ON_CONST(WM_DROPFILES                    ); // 0x0233
        _ON_CONST(WM_MDIREFRESHMENU               ); // 0x0234

#if(WINVER >= 0x0400)
        _ON_CONST(WM_IME_SETCONTEXT               ); // 0x0281
        _ON_CONST(WM_IME_NOTIFY                   ); // 0x0282
        _ON_CONST(WM_IME_CONTROL                  ); // 0x0283
        _ON_CONST(WM_IME_COMPOSITIONFULL          ); // 0x0284
        _ON_CONST(WM_IME_SELECT                   ); // 0x0285
        _ON_CONST(WM_IME_CHAR                     ); // 0x0286
#endif // WINVER >= 0x0400
#if(WINVER >= 0x0500)
        _ON_CONST(WM_IME_REQUEST                  ); // 0x0288
#endif // WINVER >= 0x0500
#if(WINVER >= 0x0400)
        _ON_CONST(WM_IME_KEYDOWN                  ); // 0x0290
        _ON_CONST(WM_IME_KEYUP                    ); // 0x0291
#endif // WINVER >= 0x0400

#if((_WIN32_WINNT >= 0x0400) || (WINVER >= 0x0500))
        _ON_CONST(WM_MOUSEHOVER                   ); // 0x02A1
        _ON_CONST(WM_MOUSELEAVE                   ); // 0x02A3
#endif
#if(WINVER >= 0x0500)
        _ON_CONST(WM_NCMOUSEHOVER                 ); // 0x02A0
        _ON_CONST(WM_NCMOUSELEAVE                 ); // 0x02A2
#endif // WINVER >= 0x0500

#if(_WIN32_WINNT >= 0x0501)
        _ON_CONST(WM_WTSSESSION_CHANGE            ); // 0x02B1

        _ON_CONST(WM_TABLET_FIRST                 ); // 0x02c0
        _ON_CONST(WM_TABLET_LAST                  ); // 0x02df
#endif // _WIN32_WINNT >= 0x0501

        _ON_CONST(WM_CUT                          ); // 0x0300
        _ON_CONST(WM_COPY                         ); // 0x0301
        _ON_CONST(WM_PASTE                        ); // 0x0302
        _ON_CONST(WM_CLEAR                        ); // 0x0303
        _ON_CONST(WM_UNDO                         ); // 0x0304
        _ON_CONST(WM_RENDERFORMAT                 ); // 0x0305
        _ON_CONST(WM_RENDERALLFORMATS             ); // 0x0306
        _ON_CONST(WM_DESTROYCLIPBOARD             ); // 0x0307
        _ON_CONST(WM_DRAWCLIPBOARD                ); // 0x0308
        _ON_CONST(WM_PAINTCLIPBOARD               ); // 0x0309
        _ON_CONST(WM_VSCROLLCLIPBOARD             ); // 0x030A
        _ON_CONST(WM_SIZECLIPBOARD                ); // 0x030B
        _ON_CONST(WM_ASKCBFORMATNAME              ); // 0x030C
        _ON_CONST(WM_CHANGECBCHAIN                ); // 0x030D
        _ON_CONST(WM_HSCROLLCLIPBOARD             ); // 0x030E
        _ON_CONST(WM_QUERYNEWPALETTE              ); // 0x030F
        _ON_CONST(WM_PALETTEISCHANGING            ); // 0x0310
        _ON_CONST(WM_PALETTECHANGED               ); // 0x0311
        _ON_CONST(WM_HOTKEY                       ); // 0x0312

#if(WINVER >= 0x0400)
        _ON_CONST(WM_PRINT                        ); // 0x0317
        _ON_CONST(WM_PRINTCLIENT                  ); // 0x0318
#endif // WINVER >= 0x0400

#if(_WIN32_WINNT >= 0x0500)
        _ON_CONST(WM_APPCOMMAND                   ); // 0x0319
#endif // _WIN32_WINNT >= 0x0500

#if(_WIN32_WINNT >= 0x0501)
        _ON_CONST(WM_THEMECHANGED                 ); // 0x031A
#endif // _WIN32_WINNT >= 0x0501


#if(_WIN32_WINNT >= 0x0501)
        _ON_CONST(WM_CLIPBOARDUPDATE              ); // 0x031D
#endif // _WIN32_WINNT >= 0x0501

#if(_WIN32_WINNT >= 0x0600)
        _ON_CONST(WM_DWMCOMPOSITIONCHANGED        ); // 0x031E
        _ON_CONST(WM_DWMNCRENDERINGCHANGED        ); // 0x031F
        _ON_CONST(WM_DWMCOLORIZATIONCOLORCHANGED  ); // 0x0320
        _ON_CONST(WM_DWMWINDOWMAXIMIZEDCHANGE     ); // 0x0321
#endif // _WIN32_WINNT >= 0x0600

#if(WINVER >= 0x0600)
        _ON_CONST(WM_GETTITLEBARINFOEX            ); // 0x033F
#endif // WINVER >= 0x0600

#if(WINVER >= 0x0400)

        _ON_CONST(WM_HANDHELDFIRST                ); // 0x0358
        _ON_CONST(WM_HANDHELDLAST                 ); // 0x035F

        _ON_CONST(WM_AFXFIRST                     ); // 0x0360
        _ON_CONST(WM_AFXLAST                      ); // 0x037F
#endif // WINVER >= 0x0400

        _ON_CONST(WM_PENWINFIRST                  ); // 0x0380
        _ON_CONST(WM_PENWINLAST                   ); // 0x038F

#if(WINVER >= 0x0400)
        _ON_CONST(WM_APP                          ); // 0x8000
#endif // WINVER >= 0x0400

//////////////////////////////////////////////////////////////////////////
// Edit messages
// 
        _ON_CONST(EM_GETSEL               ); // 0x00B0
        _ON_CONST(EM_SETSEL               ); // 0x00B1
        _ON_CONST(EM_GETRECT              ); // 0x00B2
        _ON_CONST(EM_SETRECT              ); // 0x00B3
        _ON_CONST(EM_SETRECTNP            ); // 0x00B4
        _ON_CONST(EM_SCROLL               ); // 0x00B5
        _ON_CONST(EM_LINESCROLL           ); // 0x00B6
        _ON_CONST(EM_SCROLLCARET          ); // 0x00B7
        _ON_CONST(EM_GETMODIFY            ); // 0x00B8
        _ON_CONST(EM_SETMODIFY            ); // 0x00B9
        _ON_CONST(EM_GETLINECOUNT         ); // 0x00BA
        _ON_CONST(EM_LINEINDEX            ); // 0x00BB
        _ON_CONST(EM_SETHANDLE            ); // 0x00BC
        _ON_CONST(EM_GETHANDLE            ); // 0x00BD
        _ON_CONST(EM_GETTHUMB             ); // 0x00BE
        _ON_CONST(EM_LINELENGTH           ); // 0x00C1
        _ON_CONST(EM_REPLACESEL           ); // 0x00C2
        _ON_CONST(EM_GETLINE              ); // 0x00C4
        _ON_CONST(EM_LIMITTEXT            ); // 0x00C5
        _ON_CONST(EM_CANUNDO              ); // 0x00C6
        _ON_CONST(EM_UNDO                 ); // 0x00C7
        _ON_CONST(EM_FMTLINES             ); // 0x00C8
        _ON_CONST(EM_LINEFROMCHAR         ); // 0x00C9
        _ON_CONST(EM_SETTABSTOPS          ); // 0x00CB
        _ON_CONST(EM_SETPASSWORDCHAR      ); // 0x00CC
        _ON_CONST(EM_EMPTYUNDOBUFFER      ); // 0x00CD
        _ON_CONST(EM_GETFIRSTVISIBLELINE  ); // 0x00CE
        _ON_CONST(EM_SETREADONLY          ); // 0x00CF
        _ON_CONST(EM_SETWORDBREAKPROC     ); // 0x00D0
        _ON_CONST(EM_GETWORDBREAKPROC     ); // 0x00D1
        _ON_CONST(EM_GETPASSWORDCHAR      ); // 0x00D2
#if(WINVER >= 0x0400)
        _ON_CONST(EM_SETMARGINS           ); // 0x00D3
        _ON_CONST(EM_GETMARGINS           ); // 0x00D4
        _ON_CONST(EM_GETLIMITTEXT         ); // 0x00D5
        _ON_CONST(EM_POSFROMCHAR          ); // 0x00D6
        _ON_CONST(EM_CHARFROMPOS          ); // 0x00D7
#endif // WINVER >= 0x0400

#if(WINVER >= 0x0500)
        _ON_CONST(EM_SETIMESTATUS         ); // 0x00D8
        _ON_CONST(EM_GETIMESTATUS         ); // 0x00D9
#endif // WINVER >= 0x0500

//////////////////////////////////////////////////////////////////////////
// Button messages
// 
        _ON_CONST(BM_GETCHECK        ); // 0x00F0
        _ON_CONST(BM_SETCHECK        ); // 0x00F1
        _ON_CONST(BM_GETSTATE        ); // 0x00F2
        _ON_CONST(BM_SETSTATE        ); // 0x00F3
        _ON_CONST(BM_SETSTYLE        ); // 0x00F4
#if(WINVER >= 0x0400)
        _ON_CONST(BM_CLICK           ); // 0x00F5
        _ON_CONST(BM_GETIMAGE        ); // 0x00F6
        _ON_CONST(BM_SETIMAGE        ); // 0x00F7
#endif // WINVER >= 0x0400
#if(WINVER >= 0x0600)
        _ON_CONST(BM_SETDONTCLICK    ); // 0x00F8
#endif // WINVER >= 0x0600

//////////////////////////////////////////////////////////////////////////
// Static messages
// 
        _ON_CONST(STM_SETICON         ); // 0x0170
        _ON_CONST(STM_GETICON         ); // 0x0171
#if(WINVER >= 0x0400)
        _ON_CONST(STM_SETIMAGE        ); // 0x0172
        _ON_CONST(STM_GETIMAGE        ); // 0x0173
#endif // (WINVER >= 0x0400)

//////////////////////////////////////////////////////////////////////////
// ListBox messages
// 
        _ON_CONST(LB_ADDSTRING            ); // 0x0180
        _ON_CONST(LB_INSERTSTRING         ); // 0x0181
        _ON_CONST(LB_DELETESTRING         ); // 0x0182
        _ON_CONST(LB_SELITEMRANGEEX       ); // 0x0183
        _ON_CONST(LB_RESETCONTENT         ); // 0x0184
        _ON_CONST(LB_SETSEL               ); // 0x0185
        _ON_CONST(LB_SETCURSEL            ); // 0x0186
        _ON_CONST(LB_GETSEL               ); // 0x0187
        _ON_CONST(LB_GETCURSEL            ); // 0x0188
        _ON_CONST(LB_GETTEXT              ); // 0x0189
        _ON_CONST(LB_GETTEXTLEN           ); // 0x018A
        _ON_CONST(LB_GETCOUNT             ); // 0x018B
        _ON_CONST(LB_SELECTSTRING         ); // 0x018C
        _ON_CONST(LB_DIR                  ); // 0x018D
        _ON_CONST(LB_GETTOPINDEX          ); // 0x018E
        _ON_CONST(LB_FINDSTRING           ); // 0x018F
        _ON_CONST(LB_GETSELCOUNT          ); // 0x0190
        _ON_CONST(LB_GETSELITEMS          ); // 0x0191
        _ON_CONST(LB_SETTABSTOPS          ); // 0x0192
        _ON_CONST(LB_GETHORIZONTALEXTENT  ); // 0x0193
        _ON_CONST(LB_SETHORIZONTALEXTENT  ); // 0x0194
        _ON_CONST(LB_SETCOLUMNWIDTH       ); // 0x0195
        _ON_CONST(LB_ADDFILE              ); // 0x0196
        _ON_CONST(LB_SETTOPINDEX          ); // 0x0197
        _ON_CONST(LB_GETITEMRECT          ); // 0x0198
        _ON_CONST(LB_GETITEMDATA          ); // 0x0199
        _ON_CONST(LB_SETITEMDATA          ); // 0x019A
        _ON_CONST(LB_SELITEMRANGE         ); // 0x019B
        _ON_CONST(LB_SETANCHORINDEX       ); // 0x019C
        _ON_CONST(LB_GETANCHORINDEX       ); // 0x019D
        _ON_CONST(LB_SETCARETINDEX        ); // 0x019E
        _ON_CONST(LB_GETCARETINDEX        ); // 0x019F
        _ON_CONST(LB_SETITEMHEIGHT        ); // 0x01A0
        _ON_CONST(LB_GETITEMHEIGHT        ); // 0x01A1
        _ON_CONST(LB_FINDSTRINGEXACT      ); // 0x01A2
        _ON_CONST(LB_SETLOCALE            ); // 0x01A5
        _ON_CONST(LB_GETLOCALE            ); // 0x01A6
        _ON_CONST(LB_SETCOUNT             ); // 0x01A7
#if(WINVER >= 0x0400)
        _ON_CONST(LB_INITSTORAGE          ); // 0x01A8
        _ON_CONST(LB_ITEMFROMPOINT        ); // 0x01A9
#endif // WINVER >= 0x0400
#if defined(_WIN32_WCE) && (_WIN32_WCE >= 0x0400)
        _ON_CONST(LB_MULTIPLEADDSTRING    ); // 0x01B1
#endif

#if(_WIN32_WINNT >= 0x0501)
        _ON_CONST(LB_GETLISTBOXINFO       ); // 0x01B2
#endif // _WIN32_WINNT >= 0x0501

//////////////////////////////////////////////////////////////////////////
// ComboBox messages
// 
        _ON_CONST(CB_GETEDITSEL               ); // 0x0140
        _ON_CONST(CB_LIMITTEXT                ); // 0x0141
        _ON_CONST(CB_SETEDITSEL               ); // 0x0142
        _ON_CONST(CB_ADDSTRING                ); // 0x0143
        _ON_CONST(CB_DELETESTRING             ); // 0x0144
        _ON_CONST(CB_DIR                      ); // 0x0145
        _ON_CONST(CB_GETCOUNT                 ); // 0x0146
        _ON_CONST(CB_GETCURSEL                ); // 0x0147
        _ON_CONST(CB_GETLBTEXT                ); // 0x0148
        _ON_CONST(CB_GETLBTEXTLEN             ); // 0x0149
        _ON_CONST(CB_INSERTSTRING             ); // 0x014A
        _ON_CONST(CB_RESETCONTENT             ); // 0x014B
        _ON_CONST(CB_FINDSTRING               ); // 0x014C
        _ON_CONST(CB_SELECTSTRING             ); // 0x014D
        _ON_CONST(CB_SETCURSEL                ); // 0x014E
        _ON_CONST(CB_SHOWDROPDOWN             ); // 0x014F
        _ON_CONST(CB_GETITEMDATA              ); // 0x0150
        _ON_CONST(CB_SETITEMDATA              ); // 0x0151
        _ON_CONST(CB_GETDROPPEDCONTROLRECT    ); // 0x0152
        _ON_CONST(CB_SETITEMHEIGHT            ); // 0x0153
        _ON_CONST(CB_GETITEMHEIGHT            ); // 0x0154
        _ON_CONST(CB_SETEXTENDEDUI            ); // 0x0155
        _ON_CONST(CB_GETEXTENDEDUI            ); // 0x0156
        _ON_CONST(CB_GETDROPPEDSTATE          ); // 0x0157
        _ON_CONST(CB_FINDSTRINGEXACT          ); // 0x0158
        _ON_CONST(CB_SETLOCALE                ); // 0x0159
        _ON_CONST(CB_GETLOCALE                ); // 0x015A
#if(WINVER >= 0x0400)
        _ON_CONST(CB_GETTOPINDEX              ); // 0x015b
        _ON_CONST(CB_SETTOPINDEX              ); // 0x015c
        _ON_CONST(CB_GETHORIZONTALEXTENT      ); // 0x015d
        _ON_CONST(CB_SETHORIZONTALEXTENT      ); // 0x015e
        _ON_CONST(CB_GETDROPPEDWIDTH          ); // 0x015f
        _ON_CONST(CB_SETDROPPEDWIDTH          ); // 0x0160
        _ON_CONST(CB_INITSTORAGE              ); // 0x0161
#if defined(_WIN32_WCE) && (_WIN32_WCE >= 0x0400)
        _ON_CONST(CB_MULTIPLEADDSTRING        ); // 0x0163
#endif
#endif // WINVER >= 0x0400

#if(_WIN32_WINNT >= 0x0501)
        _ON_CONST(CB_GETCOMBOBOXINFO          ); // 0x0164
#endif // _WIN32_WINNT >= 0x0501

//////////////////////////////////////////////////////////////////////////
// ScrollBar messages
// 
        _ON_CONST(SBM_SETPOS                  ); // 0x00E0 // not in win3.1
        _ON_CONST(SBM_GETPOS                  ); // 0x00E1 // not in win3.1
        _ON_CONST(SBM_SETRANGE                ); // 0x00E2 // not in win3.1
        _ON_CONST(SBM_SETRANGEREDRAW          ); // 0x00E6 // not in win3.1
        _ON_CONST(SBM_GETRANGE                ); // 0x00E3 // not in win3.1
        _ON_CONST(SBM_ENABLE_ARROWS           ); // 0x00E4 // not in win3.1
#if(WINVER >= 0x0400)
        _ON_CONST(SBM_SETSCROLLINFO           ); // 0x00E9
        _ON_CONST(SBM_GETSCROLLINFO           ); // 0x00EA
#endif // WINVER >= 0x0400

#if(_WIN32_WINNT >= 0x0501)
        _ON_CONST(SBM_GETSCROLLBARINFO        ); // 0x00EB
#endif // _WIN32_WINNT >= 0x0501

