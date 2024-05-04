#pragma once

#include "rectz.h"
#include <atlgdi.h>

struct CTheme;

struct WindowRects
{
    int     m_BorderSize = {0};
    CRect     m_rcBorder = {};
    CRect      m_rcFrame = {};
    CRect       m_rcCapt = {};
    CRect       m_rcMenu = {};
    CRect  m_rcWorkspace = {};
    CRect     m_rcScroll = {};
    CRect    m_rcMessage = {};
    CRect        m_rcURL = {};
    CRect     m_rcButton = {};
    CRect    m_rcTooltip = {};
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

    void CalcRects(CRect const& rc, UINT captFlags, WindowRects& target) const;

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
    void DrawWindow(CDCHandle dc, DrawWindowArgs const& params) const;
    void DrawToolTip(CDCHandle dc, CRect const& rc, ATL::CStringW&& tooltip) const;

private:
    struct StaticInit;

    CTheme const& m_Theme;
};
