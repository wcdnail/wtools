#pragma once

#include "rectz.h"
#include <atlgdi.h>

struct CTheme;

enum WindowRectIndex: int
{
    WR_Invalid = -1,
    WR_Tooltip = 0,
    WR_Button,
    WR_Hyperlink,
    WR_Message,
    WR_Scroll,
    WR_WinText,
    WR_Workspace,
    WR_MenuSelected,
    WR_MenuDisabled,
    WR_MenuItem,
    WR_Menu,
    WR_Caption,
    WR_Frame,
    WR_Border,
    WR_Count
};

using WindowRects = CRect[WR_Count];

enum WinTextFalgs : UINT
{
     WT_Usual = 0x00000000,
    WT_Select = 0x00000001,
       WT_URL = 0x00000002,
};

struct WinText
{
    UINT           flags = {WT_Usual};
    ATL::CStringW   text = {};
};

struct WindowText
{
    WinText const*  line = {nullptr};
    int        lineCount = {0};
    UINT           flags = {0};
};

struct DrawWindowArgs
{
    ATL::CStringW caption;
    UINT        captFlags;
    HMENU           hMenu;
    int      selectedMenu;
    WindowText       text;
};

class CDrawRoutine
{
public:
    static UINT GetDrawItemFrameType(UINT nCtlType);
    static UINT ConvDrawItemState(UINT diState);

    ~CDrawRoutine();
    CDrawRoutine(CTheme const& theme);

    void CalcRects(CRect const& rc, UINT captFlags, WindowRects& target);

    void DrawBorder(WTL::CDCHandle dc, CRect const& rcParam, int borderWidth, HBRUSH hBrush) const;
    void DrawEdge(WTL::CDCHandle dc, CRect& pRect, UINT edge, UINT uFlags) const;
    void DrawFrameButton(WTL::CDCHandle dc, CRect& rcParam, UINT uState) const;
    void DrawFrameCaption(WTL::CDCHandle dc, CRect& rcParam, UINT uFlags);
    void DrawFrameScroll(WTL::CDCHandle dc, CRect& rcParam, UINT uFlags);
    void DrawFrameControl(WTL::CDCHandle dc, CRect& rcParam, UINT uType, UINT uState);
    LONG DrawCaptionButtons(WTL::CDCHandle dc, CRect const& rcCaption, bool withMinMax, UINT uFlags);
    void DrawCaption(WTL::CDCHandle dc, CRect const& rcParam, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags) const;
    void DrawMenuText(WTL::CDCHandle hdc, PCWSTR text, CRect& rc, UINT format, int color) const;
    void DrawDisabledMenuText(WTL::CDCHandle dc, PCWSTR text, CRect& rc, UINT format) const;
    void DrawMenuBar(WTL::CDCHandle dc, CRect const& rc, HMENU hMenu, HFONT hFont, int selIt, WindowRects& rects) const;
    void DrawScrollbar(WTL::CDCHandle dc, CRect const& rcParam, BOOL enabled);
    void DrawToolTip(WTL::CDCHandle dc, CRect& rcParam, ATL::CStringW&& tooltip) const;
    void DrawDesktopIcon(WTL::CDCHandle dc, CRect const& rcParam, ATL::CStringW&& text, bool drawCursor) const;
    void DrawWindow(WTL::CDCHandle dc, DrawWindowArgs const& params, WindowRects& rects);

private:
    struct StaticInit;

    CTheme const&  m_Theme;
    int       m_BorderSize;
    WTL::CFont m_ftMarlett;
};
