_ON_BITMASK(LVS_ICON           ); /* 0x0000 */
_ON_BITMASK(LVS_REPORT         ); /* 0x0001 */
_ON_BITMASK(LVS_SMALLICON      ); /* 0x0002 */
_ON_BITMASK(LVS_LIST           ); /* 0x0003 */
_ON_BITMASK(LVS_TYPEMASK       ); /* 0x0003 */
_ON_BITMASK(LVS_SINGLESEL      ); /* 0x0004 */
_ON_BITMASK(LVS_SHOWSELALWAYS  ); /* 0x0008 */
_ON_BITMASK(LVS_SORTASCENDING  ); /* 0x0010 */
_ON_BITMASK(LVS_SORTDESCENDING ); /* 0x0020 */
_ON_BITMASK(LVS_SHAREIMAGELISTS); /* 0x0040 */
_ON_BITMASK(LVS_NOLABELWRAP    ); /* 0x0080 */
_ON_BITMASK(LVS_AUTOARRANGE    ); /* 0x0100 */
_ON_BITMASK(LVS_EDITLABELS     ); /* 0x0200 */
#if (_WIN32_IE >= 0x0300)
_ON_BITMASK(LVS_OWNERDATA      ); /* 0x1000 */
#endif
_ON_BITMASK(LVS_NOSCROLL       ); /* 0x2000 */
_ON_BITMASK(LVS_TYPESTYLEMASK  ); /* 0xfc00 */
//_ON_BITMASK(LVS_ALIGNTOP     ); /* 0x0000 */
_ON_BITMASK(LVS_ALIGNLEFT      ); /* 0x0800 */
_ON_BITMASK(LVS_ALIGNMASK      ); /* 0x0c00 */
_ON_BITMASK(LVS_OWNERDRAWFIXED ); /* 0x0400 */
_ON_BITMASK(LVS_NOCOLUMNHEADER ); /* 0x4000 */
_ON_BITMASK(LVS_NOSORTHEADER   ); /* 0x8000 */
