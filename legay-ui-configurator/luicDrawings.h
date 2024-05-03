#pragma once

#include "rectz.h"
#include <atlgdi.h>

struct CTheme;

struct WindowRects
{
    int     m_BorderSize = { 0 };
    CRect     m_rcBorder = {};
    CRect      m_rcFrame = {};
    CRect       m_rcCapt = {};
    CRect       m_rcMenu = {};
    CRect  m_rcWorkspace = {};
    CRect     m_rcScroll = {};
};

struct WinText
{
    bool        selected = { false };
    ATL::CStringW   text = {};
};

struct WindowText
{
    WinText const*  line = { nullptr };
    int        lineCount = { 0 };
    UINT           flags = { 0 };
};

struct DrawWindowArgs
{
    WindowRects&    rects;
    DRect const&    rcSrc;
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
    LONG DrawCaptionButtons(CDCHandle dc, CRect const& rcCaption, bool withMinMax, UINT uFlags) const;
    void DrawCaption(CDCHandle dc, CRect const& rcParam, HFONT hFont, HICON hIcon, PCWSTR str, UINT uFlags) const;
    void DrawMenuText(CDCHandle hdc, PCWSTR text, CRect& rc, UINT format, int color) const;
    void DrawDisabledMenuText(CDCHandle dc, PCWSTR text, CRect& rc, UINT format) const;
    void DrawMenuBar(CDCHandle dc, CRect const& rc, HMENU hMenu, HFONT hFont, int selectedItem) const;
    void DrawScrollbar(CDCHandle dc, CRect const& rcParam, BOOL enabled) const;
    void DrawWindow(CDCHandle dc, DrawWindowArgs const& params) const;

private:
    struct StaticInit;

    CTheme const& m_Theme;
};
