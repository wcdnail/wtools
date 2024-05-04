#pragma once

#include "rectz.h"
#include <atlgdi.h>

struct CTheme;

enum WindowRectIndex: int
{
    WR_Border = 0,
    WR_Frame,
    WR_Caption,
    WR_Menu,
    WR_Workspace,
    WR_Scroll,
    WR_Message,
    WR_Hyperlink,
    WR_Button,
    WR_Tooltip,
    WR_Count
};

struct WindowRects
{
    CRect rect[WR_Count];
};

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
    WindowRects&    rects;
    DRect           drect;
    ATL::CStringW caption;
    UINT        captFlags;
    HMENU           hMenu;
    int      selectedMenu;
    WindowText       text;
};

class CDrawRoutine
{
public:
    ~CDrawRoutine();
    CDrawRoutine(CTheme const& theme);

    void CalcRects(CRect const& rc, UINT captFlags, WindowRects& target);

    void DrawBorder(CDCHandle dc, CRect const& rcParam, int borderWidth, HBRUSH hBrush) const;
    void DrawEdge(CDCHandle dc, CRect& pRect, UINT edge, UINT uFlags) const;
    void DrawFrameButton(CDCHandle dc, CRect& rcParam, UINT uState) const;
    void DrawFrameCaption(CDCHandle dc, CRect& rcParam, UINT uFlags, CFont& fnMarlett) const;
    void DrawFrameScroll(CDCHandle dc, CRect& rcParam, UINT uFlags, CFont& fnMarlett) const;
    void DrawFrameControl(CDCHandle dc, CRect& rcParam, UINT uType, UINT uState, CFont& fnMarlett) const;
    LONG DrawCaptionButtons(CDCHandle dc, CRect const& rcCaption, bool withMinMax, UINT uFlags) const;
    void DrawCaption(CDCHandle dc, CRect const& rcParam, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags) const;
    void DrawMenuText(CDCHandle hdc, PCWSTR text, CRect& rc, UINT format, int color) const;
    void DrawDisabledMenuText(CDCHandle dc, PCWSTR text, CRect& rc, UINT format) const;
    void DrawMenuBar(CDCHandle dc, CRect const& rc, HMENU hMenu, HFONT hFont, int selectedItem) const;
    void DrawScrollbar(CDCHandle dc, CRect const& rcParam, BOOL enabled) const;
    void DrawToolTip(CDCHandle dc, CRect const& rc, ATL::CStringW&& tooltip) const;
    void DrawDesktopIcon(CDCHandle dc, CRect const& rcParam, ATL::CStringW&& text, bool drawCursor) const;
    void DrawWindow(CDCHandle dc, DrawWindowArgs const& params) const;

private:
    struct StaticInit;

    CTheme const& m_Theme;
    int      m_BorderSize;
};
