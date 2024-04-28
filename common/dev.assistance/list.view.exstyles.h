_ON_BITMASK(LVS_EX_GRIDLINES        ); /* 0x00000001 */ 
_ON_BITMASK(LVS_EX_SUBITEMIMAGES    ); /* 0x00000002 */ 
_ON_BITMASK(LVS_EX_CHECKBOXES       ); /* 0x00000004 */ 
_ON_BITMASK(LVS_EX_TRACKSELECT      ); /* 0x00000008 */ 
_ON_BITMASK(LVS_EX_HEADERDRAGDROP   ); /* 0x00000010 */ 
_ON_BITMASK(LVS_EX_FULLROWSELECT    ); /* 0x00000020 */ // applies to report mode only
_ON_BITMASK(LVS_EX_ONECLICKACTIVATE ); /* 0x00000040 */ 
_ON_BITMASK(LVS_EX_TWOCLICKACTIVATE ); /* 0x00000080 */ 
#if (_WIN32_IE >= 0x0400)
_ON_BITMASK(LVS_EX_FLATSB           );
_ON_BITMASK(LVS_EX_REGIONAL         ); /* 0x00000200 */ 
_ON_BITMASK(LVS_EX_INFOTIP          ); /* 0x00000400 */ // listview does InfoTips for you
_ON_BITMASK(LVS_EX_UNDERLINEHOT     ); /* 0x00000800 */ 
_ON_BITMASK(LVS_EX_UNDERLINECOLD    ); /* 0x00001000 */ 
_ON_BITMASK(LVS_EX_MULTIWORKAREAS   ); /* 0x00002000 */ 
#endif
#if (_WIN32_IE >= 0x0500)
_ON_BITMASK(LVS_EX_LABELTIP         ); /* 0x00004000 */ // listview unfolds partly hidden labels if it does not have infotip text
_ON_BITMASK(LVS_EX_BORDERSELECT     ); /* 0x00008000 */ // border selection style instead of highlight
#endif  // End (_WIN32_IE >= 0x0500)
#if (_WIN32_WINNT >= 0x0501)
_ON_BITMASK(LVS_EX_DOUBLEBUFFER     ); /* 0x00010000 */ 
_ON_BITMASK(LVS_EX_HIDELABELS       ); /* 0x00020000 */ 
_ON_BITMASK(LVS_EX_SINGLEROW        ); /* 0x00040000 */ 
_ON_BITMASK(LVS_EX_SNAPTOGRID       ); /* 0x00080000 */ // Icons automatically snap to grid.
_ON_BITMASK(LVS_EX_SIMPLESELECT     ); /* 0x00100000 */ // Also changes overlay rendering to top right for icon mode.
#endif
#if _WIN32_WINNT >= 0x0600
_ON_BITMASK(LVS_EX_JUSTIFYCOLUMNS   ); /* 0x00200000 */ // Icons are lined up in columns that use up the whole view area.
_ON_BITMASK(LVS_EX_TRANSPARENTBKGND ); /* 0x00400000 */ // Background is painted by the parent via WM_PRINTCLIENT
_ON_BITMASK(LVS_EX_TRANSPARENTSHADOWTEXT); /* 0x00800000 */ // Enable shadow text on transparent backgrounds only (useful with bitmaps)
_ON_BITMASK(LVS_EX_AUTOAUTOARRANGE  ); /* 0x01000000 */ // Icons automatically arrange if no icon positions have been set
_ON_BITMASK(LVS_EX_HEADERINALLVIEWS ); /* 0x02000000 */ // Display column header in all view modes
_ON_BITMASK(LVS_EX_AUTOCHECKSELECT  ); /* 0x08000000 */ 
_ON_BITMASK(LVS_EX_AUTOSIZECOLUMNS  ); /* 0x10000000 */ 
_ON_BITMASK(LVS_EX_COLUMNSNAPPOINTS ); /* 0x40000000 */ 
_ON_BITMASK(LVS_EX_COLUMNOVERFLOW   ); /* 0x80000000 */ 
#endif
