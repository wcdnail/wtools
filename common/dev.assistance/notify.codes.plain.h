/*
 * Edit Control Notification Codes
 */
    _ON_CONST(EN_SETFOCUS );
    _ON_CONST(EN_KILLFOCUS);
    _ON_CONST(EN_CHANGE   );
    _ON_CONST(EN_UPDATE   );
    _ON_CONST(EN_ERRSPACE );
    _ON_CONST(EN_MAXTEXT  );
    _ON_CONST(EN_HSCROLL  );
    _ON_CONST(EN_VSCROLL  );
#if(_WIN32_WINNT >= 0x0500)
    _ON_CONST(EN_ALIGN_LTR_EC);
    _ON_CONST(EN_ALIGN_RTL_EC);
#endif /* _WIN32_WINNT >= 0x0500 */
/*
 * User Button Notification Codes
 */
    _ON_CONST(BN_CLICKED      );
    _ON_CONST(BN_PAINT        );
    _ON_CONST(BN_HILITE       );
    _ON_CONST(BN_UNHILITE     );
    _ON_CONST(BN_DISABLE      );
    _ON_CONST(BN_DOUBLECLICKED);
#if (WINVER >= 0x0400)
    _ON_CONST(BN_SETFOCUS     );
    _ON_CONST(BN_KILLFOCUS    );
#endif /* WINVER >= 0x0400 */
/*
 * Listbox Notification Codes
 */
    _ON_CONST(LBN_ERRSPACE );
  //_ON_CONST(LBN_SELCHANGE);
  //_ON_CONST(LBN_DBLCLK   );
  //_ON_CONST(LBN_SELCANCEL);
  //_ON_CONST(LBN_SETFOCUS );
  //_ON_CONST(LBN_KILLFOCUS);
/*
 * Combo Box Notification Codes
 */
    _ON_CONST(CBN_ERRSPACE    );
  //_ON_CONST(CBN_SELCHANGE   );
  //_ON_CONST(CBN_DBLCLK      );
  //_ON_CONST(CBN_SETFOCUS    );
  //_ON_CONST(CBN_KILLFOCUS   );
  //_ON_CONST(CBN_EDITCHANGE  );
  //_ON_CONST(CBN_EDITUPDATE  );
  //_ON_CONST(CBN_DROPDOWN    );
    _ON_CONST(CBN_CLOSEUP     );
    _ON_CONST(CBN_SELENDOK    );
    _ON_CONST(CBN_SELENDCANCEL);
/* ====== Generic WM_NOTIFY notification codes ================================= */
  //_ON_CONST(NM_OUTOFMEMORY);
  //_ON_CONST(NM_CLICK      );
    _ON_CONST(NM_DBLCLK     );
    _ON_CONST(NM_RETURN     );
    _ON_CONST(NM_RCLICK     );
    _ON_CONST(NM_RDBLCLK    );
    _ON_CONST(NM_SETFOCUS   );
    _ON_CONST(NM_KILLFOCUS  );
#if (_WIN32_IE >= 0x0300)
    _ON_CONST(NM_CUSTOMDRAW);
    _ON_CONST(NM_HOVER     );
#endif
#if (_WIN32_IE >= 0x0400)
    _ON_CONST(NM_NCHITTEST      );
    _ON_CONST(NM_KEYDOWN        );
    _ON_CONST(NM_RELEASEDCAPTURE);
    _ON_CONST(NM_SETCURSOR      );
    _ON_CONST(NM_CHAR           );
#endif
#if (_WIN32_IE >= 0x0401)
    _ON_CONST(NM_TOOLTIPSCREATED);
#endif
#if (_WIN32_IE >= 0x0500)
    _ON_CONST(NM_LDOWN       );
    _ON_CONST(NM_RDOWN       );
    _ON_CONST(NM_THEMECHANGED);
#endif
#if _WIN32_WINNT >= 0x0600
    _ON_CONST(NM_FONTCHANGED         );
    _ON_CONST(NM_CUSTOMTEXT          );
  //_ON_CONST(NM_TVSTATEIMAGECHANGING);
#endif
    _ON_CONST(TBN_GETBUTTONINFOA);
    _ON_CONST(TBN_BEGINDRAG     );
    _ON_CONST(TBN_ENDDRAG       );
    _ON_CONST(TBN_BEGINADJUST   );
    _ON_CONST(TBN_ENDADJUST     );
    _ON_CONST(TBN_RESET         );
    _ON_CONST(TBN_QUERYINSERT   );
    _ON_CONST(TBN_QUERYDELETE   );
    _ON_CONST(TBN_TOOLBARCHANGE );
    _ON_CONST(TBN_CUSTHELP      );
#if (_WIN32_IE >= 0x0300)
    _ON_CONST(TBN_DROPDOWN);
#endif
#if (_WIN32_IE >= 0x0400)
    _ON_CONST(TBN_GETOBJECT);
    _ON_CONST(TBN_HOTITEMCHANGE );
    _ON_CONST(TBN_DRAGOUT       );
    _ON_CONST(TBN_DELETINGBUTTON);
    _ON_CONST(TBN_GETDISPINFOA  );
    _ON_CONST(TBN_GETDISPINFOW  );
    _ON_CONST(TBN_GETINFOTIPA   );
    _ON_CONST(TBN_GETINFOTIPW   );
    _ON_CONST(TBN_GETBUTTONINFOW);
#if (_WIN32_IE >= 0x0500)
    _ON_CONST(TBN_RESTORE          );
    _ON_CONST(TBN_SAVE             );
    _ON_CONST(TBN_INITCUSTOMIZE    );
  //_ON_CONST(TBNRF_HIDEHELP       );
  //_ON_CONST(TBNRF_ENDCUSTOMIZE   );
#endif /* (_WIN32_IE >= 0x0500) */
#endif
/* status bar notifications */
#if (_WIN32_IE >= 0x0400)
    _ON_CONST(SBN_SIMPLEMODECHANGE);
#endif
/* PGN_SCROLL Notification Message */
    _ON_CONST(PGN_SCROLL       );
    _ON_CONST(PGN_CALCSIZE     );
    _ON_CONST(PGN_HOTITEMCHANGE);
