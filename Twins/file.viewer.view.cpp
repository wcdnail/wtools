#include "stdafx.h"
#include "file.viewer.view.h"
#include "file.entry.h"
#include "dialogz.search.text.h"
#include <dh.tracing.h>
#include <string.utils.human.size.h>
#include <twins.langs/twins.lang.strings.h>
#include <psapi.h>
#include <atlconv.h>
#include <atlgdi.h>
#include <limits>

namespace Fv
{
    enum
    {
        TimerId_UpdateMemoryStatus = 100,
        TimerEl_UpdateMemoryStatus = 500,
    };

    wchar_t Viewer::DisplayModeChar[MaxMode] = { L'T', L'B', L'X', L'^' };

    Viewer::DisplayLineFtor Viewer::DisplayLine[Viewer::MaxMode] = 
    {
      /* PlainText */ DisplayTextLine
    , /* Binary    */ DisplayBinaryLine
    , /* Hex       */ DisplayHexLine
    , /* Media     */ NULL
    };

    Viewer::ScrollFtor Viewer::ScrollTo[MaxMode] = 
    {
      /* PlainText */ ScrollInText
    , /* Binary    */ ScrollInBinary
    , /* Hex       */ ScrollInHex
    , /* Media     */ NULL
    };

    Viewer::Go2EndFtor Viewer::GoToEnd[MaxMode] = 
    {
      /* PlainText */ Go2EndInText
    , /* Binary    */ Go2EndInBinary
    , /* Hex       */ Go2EndInHex
    , /* Media     */ NULL
    };

    struct InvalidateAfter
    {
        InvalidateAfter(Viewer const& client, bool invalidate);
        ~InvalidateAfter();

    private:
        HWND Client;
        bool Invalidate;
    };

    InvalidateAfter::InvalidateAfter(Viewer const& client, bool invalidate)
        : Client((HWND)client)
        , Invalidate(invalidate)
    {}

    InvalidateAfter::~InvalidateAfter()
    {
        if (Invalidate)
            ::InvalidateRect(Client, NULL, FALSE);
    }

    Viewer::~Viewer()
    {}

    Viewer::Viewer()
        : Super()
        , State()
        , Color()
        , Font()
        , DisplayMode(PlainText)                
// ##TODO: Configure viewer disp mode"))
        , ShowStatusBar(true)                   
// ##TODO: Configure viewer status bar mode"))
        , MiceScrollAmount(4)                   
// ##TODO: Configure viewer mice scroll amount"))
        , InputStatus()
        , MemoryStatus()
        , DebugStatus()
        , StatusRect()
        , VScroller()
        , IconBig()
        , Icon()
    {}

    Viewer::DisplayState::DisplayState()
        : InBuffer()
        , CodePage(CP_ACP)
        , LineCount(0)
        , StartOffset(0)
        , DisplayOffset(0)
        , VisibleLineLen()
        , FixedLineWidth(80)                    
// ##TODO: Configure FixedLineWidth"))
        , WrappedLineWidth((SizeType)-1)        
// ##TODO: Configure WrappedLineWidth"))
        , HexLineWidth(16)                      
// ##TODO: Configure HexLineWidth"))
        , ShowSpecChars(false)                  
// ##TODO: Configure ShowSpecChars"))
    {
        ::memset(&TextMetric, 0, sizeof(TextMetric));
    }

    bool Viewer::IsLineWrapOn() const { return FileBuffer::npos != State.WrappedLineWidth; } 

    Viewer::Fonts::Fonts()
        : Default(::CreateFont(-13, 0, 0, 0, 0, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, _T("Consolas")))
        , Terminal(::CreateFont(-12, 0, 0, 0, 0, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, _T("Fixedsys")))
        , Status(::CreateFont(-11, 0, 0, 0, 0, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, _T("Lucida Console")))
    {}

    Viewer::Colors::Colors()
        : Background(0xffffff)
        , BackgroundLineNumber(0xdfdfdf)
        , BackgroundStatus(0x1e1e1e)
        , Text(0)
        , TextLineNumber(0)
        , TextStatusLine(0x90ffff)
        , TextStatusMemory(0x9090ff)
        , TextHexOffset(0x854545)
        , BackgroundHexOffset(0xefefef)
        , TextHexBytes(0x050507)
        , TextHexSymbols(0x555555)
        , BackgroundHexSymbols(0xefffff)
    {}

    void Viewer::Colors::LoadSystemValues()
    {
        Background = ::GetSysColor(COLOR_WINDOW);
        Text = ::GetSysColor(COLOR_WINDOWTEXT);
    }

    ErrorCode Viewer::Create(HWND parent, CRect& rc, UINT id)
    {
        if (NULL != m_hWnd)
            return ErrorCode(ERROR_ALREADY_INITIALIZED, std::system_category());

        Super::Create(parent, rc, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 0, id);
        HRESULT hr = ::GetLastError();
        return ErrorCode(NULL == m_hWnd ? hr : 0, std::system_category());
    }

    void Viewer::Reset(bool invalidate)
    {
        InvalidateAfter me(*this, invalidate);

        InputStatus.clear();
        MemoryStatus.clear();
        DebugStatus.clear();
        State.LineCount = 0;

        SCROLLINFO si = { sizeof(si), SIF_PAGE | SIF_RANGE, -1, -1, static_cast<UINT>(-1), 0, 0 };
        VScroller.SetScrollInfo(&si, FALSE);
        VScroller.SetScrollPos(-1, TRUE);
    }

    CIcon const& Viewer::GetBigIcon() const { return IconBig; }
    CIcon const& Viewer::GetSmallIcon() const { return Icon; }
    Path const& Viewer::GetPath() const { return State.InBuffer.GetPath(); }

    ErrorCode Viewer::Open(Fl::Entry const& entry, SizeType bs /*= 1024 * 1024*/)
    {
        ErrorCode error = Open(entry.GetPath().c_str(), bs);

        if (!error)
        {
            IconBig.Attach(entry.LoadShellIcon(SHGFI_ADDOVERLAYS | SHGFI_LARGEICON));
            Icon.Attach(entry.LoadShellIcon(SHGFI_ADDOVERLAYS | SHGFI_SMALLICON));
        }

        return error;
    }

    ErrorCode Viewer::Open(Path const& path, SizeType bs /*= 1024 * 1024*/)
    {
        ErrorCode error = State.InBuffer.Open(path, bs);

        if (!error)
        {
            Reset(m_hWnd != NULL);
            State.InBuffer.GetStatus(InputStatus);
        }
        else
        {
            wchar_t buffer[2048] = {0};
            ::_snwprintf_s(buffer, _countof(buffer)-1, L"`%s` (%d) %s ", path.c_str()
                , error.value(), CA2W(error.message().c_str()).m_psz);

            DebugStatus = buffer;
        }

        return error;
    }

    int Viewer::OnCreate(LPCREATESTRUCT cs)
    {
        CRect rc;
        GetClientRect(rc);
        
        CRect rcVScroll(rc);
        rcVScroll.left = rc.right - ::GetSystemMetrics(SM_CXVSCROLL) - 2;
        VScroller.Create(m_hWnd, rcVScroll, NULL, WS_CHILD | SBS_VERT, 0, ID_VSCROLL);

        Color.LoadSystemValues();
        SetTimer(TimerId_UpdateMemoryStatus, TimerEl_UpdateMemoryStatus);
        SetFocus();

        DlgResize_Init(false, false);

        SetDisplayMode(State.InBuffer.IsBinary() ? Binary : PlainText);
        return 0;
    }

    void Viewer::OnDestroy()
    {
        KillTimer(TimerId_UpdateMemoryStatus);
        SetMsgHandled(FALSE);
    }

    void Viewer::OnTimer(UINT_PTR id)
    {
        if (TimerId_UpdateMemoryStatus == id)
        {
            PROCESS_MEMORY_COUNTERS pmc = {};
            AutoHandle self(::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId()), CloseHandle);
            ::GetProcessMemoryInfo(self.get(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc));

            wchar_t buffer[64] = {0};
            ::_snwprintf_s(buffer, _countof(buffer)-1, L"[%.1fK]", (double)pmc.WorkingSetSize / 1024.);
            MemoryStatus = buffer;

            InvalidateStatus();
        }
    }

    void Viewer::InvalidateStatus()
    {
        if (ShowStatusBar && !StatusRect.IsRectNull())
            ::InvalidateRect(m_hWnd, StatusRect, FALSE);
    }

    void Viewer::DrawStatusLine(CDCHandle dc, CRect const& rcView, CRect const& rc) const
    {
        if (!ShowStatusBar)
            return ;

        CRect rcStatus(0, rcView.bottom, rcView.right, rc.bottom);
        dc.FillSolidRect(rcStatus, Color.BackgroundStatus);

        if (InputStatus.empty() && MemoryStatus.empty() && DebugStatus.empty())
            return ;

        HFONT lfont = dc.SelectFont(Font.Status);
        COLORREF lcol = dc.SetTextColor(Color.TextStatusLine);
        COLORREF lbol = dc.SetBkColor(Color.BackgroundStatus);
        int bmode = dc.SetBkMode(OPAQUE);

        CRect rcStatusText(rcStatus);
        rcStatusText.DeflateRect(4, 2);

        WString status;
        if (!InputStatus.empty())
        {
            wchar_t buffer[256] = {0};
            ::_snwprintf_s(buffer, _countof(buffer)-1, L"%c%c%c %s"
                , DisplayModeChar[DisplayMode]
                , IsLineWrapOn() ? L'W' : L'-'
                , State.ShowSpecChars ? L'S' : L'-'
                , InputStatus.c_str()
                );

            status += buffer;
        }

        if (!DebugStatus.empty())
            status += L" " + DebugStatus;

        ::DrawTextW(dc, status.c_str(), (int)status.length(), rcStatusText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

        if (!MemoryStatus.empty())
        {
            dc.SetTextColor(Color.TextStatusMemory);
            ::DrawTextW(dc, MemoryStatus.c_str(), (int)MemoryStatus.length(), rcStatusText, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
        }

        dc.SetBkMode(bmode);
        dc.SetBkColor(lbol);
        dc.SetTextColor(lcol);
        dc.SelectFont(lfont);

        StatusRect = rcStatusText;
    }

    CRect Viewer::GetViewRect(CRect const& rc) const
    {
        CRect rv(rc);

        if (VScroller.IsWindowVisible())
        {
            CRect rcvs;
            VScroller.GetWindowRect(rcvs);
            rv.right -= rcvs.Width();
        }

        if (ShowStatusBar)
        {
            LOGFONT lf = {};
            Font.Status.GetLogFont(&lf);
            rv.bottom -= (abs(lf.lfHeight) + 4);
        }

        return rv;
    }

    BOOL Viewer::OnEraseBkgnd(CDCHandle dc) const
    {
        CRect rc;
        GetClientRect(rc);
        dc.FillSolidRect(rc, Color.Background);
        return TRUE;
    }

    void Viewer::AdjustFontSize(int dy, bool invalidate)
    {
        InvalidateAfter me(*this, invalidate);

        CFont& dfont = GetDisplayFont();

        LOGFONT lf = {};
        if (dfont.GetLogFont(&lf))
        {
            lf.lfHeight -= dy;
            if (lf.lfHeight < 0)
            {
                CFont temp;
                if (temp.CreateFontIndirect(&lf))
                    dfont.Attach(temp.Detach());
            }
        }
    }

    CFont& Viewer::GetDisplayFont() const
    {
        return Font.Default;
    }

    void Viewer::OnPaint(CDCHandle) const
    {
        CRect rc;
        GetClientRect(rc);

        CPaintDC paintDc(*this);
        CMemoryDC memDc(paintDc, rc);
        CDCHandle dc(memDc);

        dc.FillSolidRect(rc, Color.Background);

        dc.SetTextColor(Color.Text);
        dc.SetBkMode(TRANSPARENT);

        HFONT lobj = dc.SelectFont(GetDisplayFont());
        dc.GetTextMetrics(&State.TextMetric);

        CRect rcView = GetViewRect(rc);

        State.DisplayOffset = State.StartOffset;
        int lineCy = State.TextMetric.tmHeight;
        int symbolCx = State.TextMetric.tmAveCharWidth;
        if (State.InBuffer.Ok() && (lineCy > 0) && (symbolCx > 0))
        {
            State.LineCount = rcView.Height() / lineCy;
            State.VisibleLineLen = rcView.Width() / symbolCx + 1;

            CRect rcLine(rcView);
            rcLine.bottom = rcLine.top + lineCy;
            for (int i=0; i<State.LineCount; i++)
            {
                DrawLine(dc, rcLine);
                rcLine.top = rcLine.bottom;
                rcLine.bottom = rcLine.top + lineCy;
            }
        }

        DrawStatusLine(dc, rcView, rc);
        dc.SelectFont(lobj);
    }

    void Viewer::DrawLine(CDCHandle dc, CRect& rcLine) const
    {
        if (NULL != DisplayLine[DisplayMode])
            DisplayLine[DisplayMode](dc, rcLine, State, Color);
    }

    void Viewer::ToggleLineWrap(bool invalidate)
    {
        if (PlainText == DisplayMode)
        {
            InvalidateAfter me(*this, invalidate);
            State.WrappedLineWidth = IsLineWrapOn() ? FileBuffer::npos : State.FixedLineWidth;
        }
    }

    void Viewer::ToggleShowSpec(bool invalidate)
    {
        InvalidateAfter me(*this, invalidate);
        State.ShowSpecChars = !State.ShowSpecChars;
    }

    void Viewer::RedirectMessageToParent(bool async) const
    {
        if (async)
            ::PostMessage(GetParent(), m_pCurrentMsg->message, m_pCurrentMsg->wParam, m_pCurrentMsg->lParam);
        else
            ::SendMessage(GetParent(), m_pCurrentMsg->message, m_pCurrentMsg->wParam, m_pCurrentMsg->lParam);
    }

    void Viewer::OnKeyDown(UINT code, UINT, UINT flags)
    {
        bool ctrl  = (0 != (0x8000 & ::GetAsyncKeyState(VK_CONTROL)));
        bool alt   = (0 != (0x8000 & ::GetAsyncKeyState(VK_MENU)));
        bool shift = (0 != (0x8000 & ::GetAsyncKeyState(VK_SHIFT)));

        switch (code)
        {
        case '9': case '8': 
        case '7': case '6': 
        case '5': case '4': 
        case '3': case '2':
        case '1':       SetDisplayMode((int)(code - (UINT)'1')); break;
        case 'W':       if (ctrl) { RedirectMessageToParent(false); } else { ToggleLineWrap(); } break;
        case 'S':       ToggleShowSpec(); break;
        case VK_UP:     Scroll(-1); break;
        case VK_DOWN:   Scroll(1); break;
        case VK_PRIOR:  Scroll(-State.LineCount); break;
        case VK_NEXT:   Scroll(State.LineCount); break;
        case VK_HOME:   if (ctrl) { Go2Begin(); } break;
        case VK_END:    if (ctrl) { Go2End(); } break;
        case VK_F7:     StartSearchInFile(); break;

        default:
            RedirectMessageToParent(false);
        }
    }

    BOOL Viewer::OnMouseWheel(UINT flags, short delta, CPoint point)
    {
        bool ctrl  = (0 != (0x8000 & ::GetAsyncKeyState(VK_CONTROL)));

        if (ctrl)
            AdjustFontSize(delta > 0 ? 1 : -1);
        else
            Scroll(delta > 0 ? -MiceScrollAmount : MiceScrollAmount);

        return TRUE;
    }

    void Viewer::Scroll(int amount, bool invalidate)
    {
        if (NULL != ScrollTo[DisplayMode])
        {
            InvalidateAfter me(*this, invalidate);
            State.StartOffset = ScrollTo[DisplayMode](State, amount);
        }
    }

    void Viewer::Go2Begin(bool invalidate)
    {
        InvalidateAfter me(*this, invalidate);
        State.StartOffset = 0;
    }

    void Viewer::Go2End(bool invalidate)
    {
        if (NULL != GoToEnd[DisplayMode])
        {
            InvalidateAfter me(*this, invalidate);
            State.StartOffset = GoToEnd[DisplayMode](State);
        }
    }

    void Viewer::SetDisplayMode(int mode, bool invalidate)
    {
        if ((mode >= PlainText) && (mode < MaxMode))
        {
            InvalidateAfter me(*this, invalidate);
            DisplayMode = mode;
        }
    }

    void Viewer::DisplayTextLine(CDCHandle dc, CRect const& rc, DisplayState& s, Colors const& c)
    {
        SizeType end = s.InBuffer.GetLineEnd(s.DisplayOffset, s.WrappedLineWidth);
        WString line = s.InBuffer.GetTextLine(s.DisplayOffset, end, s.CodePage, s.ShowSpecChars);
        s.DisplayOffset = end;

        CRect rcLine = rc;
        ::DrawTextW(dc, line.c_str(), min(s.VisibleLineLen, (int)line.length()), rcLine
            , DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER);
    }

    SizeType Viewer::ScrollInText(DisplayState& s, int amount)
    {
        bool up = amount < 0;
        int  dy = abs(amount);
        SizeType offset = s.StartOffset;
        if (up) {
            offset = s.InBuffer.FindPrevLine(dy, offset);
            // ##TODO: Adjust offset in line-wrap mode
        }
        else {
            offset = s.InBuffer.FindNextLine(dy, offset, s.WrappedLineWidth);
            // ##TODO: Adjust offset at end of file
        }
        return offset;
    }

    SizeType Viewer::Go2EndInText(DisplayState& s) 
    {
        SizeType offset = s.InBuffer.FindPrevLineFromEnd(s.LineCount); 
        // ##TODO: Adjust offset in line-wrap mode
        return offset;
    }

    void Viewer::DisplayBinaryLine(CDCHandle dc, CRect const& rc, DisplayState& s, Colors const& c)
    {
        SizeType end = s.InBuffer.GetFixedLineEnd(s.DisplayOffset, s.FixedLineWidth);
        WString line = s.InBuffer.GetTextLine(s.DisplayOffset, end, s.CodePage, s.ShowSpecChars);
        s.DisplayOffset = end;
        
        CRect rcLine = rc;
        ::DrawTextW(dc, line.c_str(), min(s.VisibleLineLen, (int)line.length()), rcLine,
            DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER);
    }

    SizeType Viewer::ScrollFixed(DisplayState& s, int amount, int stride)
    {
        bool up = amount < 0;
        int dy = abs(amount);
        SizeType    inc = dy * stride;
        SizeType offset = s.StartOffset;
        if (up) {
            if (inc > offset) {
                inc = offset;
            }
            offset -= inc;
        }
        else {
            offset += inc;
            s.InBuffer.CheckFileLimit(offset, s.LineCount * stride);
        }
        return offset;
    }

    SizeType Viewer::Go2EndFixed(DisplayState& s, int stride) 
    {
        SizeType offset = s.InBuffer.GetFileSize(); 
        s.InBuffer.CheckFileLimit(offset, s.LineCount * stride);
        return offset;
    }

    SizeType Viewer::ScrollInBinary(DisplayState& s, int amount) { return ScrollFixed(s, amount, s.FixedLineWidth); }
    SizeType Viewer::Go2EndInBinary(DisplayState& s) { return Go2EndFixed(s, s.FixedLineWidth); }
    SizeType Viewer::ScrollInHex(DisplayState& s, int amount) { return ScrollFixed(s, amount, s.HexLineWidth); }
    SizeType Viewer::Go2EndInHex(DisplayState& s) { return Go2EndFixed(s, s.HexLineWidth); }

    void Viewer::DisplayHexLine(CDCHandle dc, CRect const& rc, DisplayState& s, Colors const& c)
    {
        SizeType beg = s.DisplayOffset;
        if (beg >= s.InBuffer.GetFileSize()) {
            return;
        }
        const int aveSymbolWidth = s.TextMetric.tmAveCharWidth;

        WString offsetText;
        wchar_t buffer[32] = {0};
        ::_snwprintf_s(buffer, _countof(buffer)-1, L"%08x%08x",
            static_cast<UINT>(beg >> 32),
            static_cast<UINT>(beg & 0xffffffff));
        offsetText = buffer;

        SizeType end = s.InBuffer.GetFixedLineEnd(beg, s.HexLineWidth);
        WString  hex = s.InBuffer.GetHexLine(beg, end);
        WString text = s.InBuffer.GetTextLine(beg, end, s.CodePage, true);
        s.DisplayOffset = end;

        COLORREF lcol = dc.SetTextColor(c.TextHexOffset);
        int lmode = dc.SetBkMode(TRANSPARENT);

        CRect rcOffset = rc;
        rcOffset.right = rcOffset.left + aveSymbolWidth * 16 + 6;
        dc.FillSolidRect(rcOffset, c.BackgroundHexOffset);
        rcOffset.right -= 4;

        ::DrawTextW(dc, offsetText.c_str(), (int)offsetText.length()
            , rcOffset, DT_RIGHT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER);

        CRect rcHex = rc;
        rcHex.left = rcOffset.right + 8;
        rcHex.right = rcHex.left + aveSymbolWidth * s.HexLineWidth * 3 + 2;

        dc.SetTextColor(c.TextHexBytes);
        ::DrawTextW(dc, hex.c_str(), (int)hex.length()
            , rcHex, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER);

        CRect rcText = rc;
        rcText.left = rcHex.right + 4;
        rcText.right = rc.right;
        dc.FillSolidRect(rcText, c.BackgroundHexSymbols);
        rcText.left += 4;

        dc.SetTextColor(c.TextHexSymbols);
        ::DrawTextW(dc, text.c_str(), (int)text.length()
            , rcText, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER);

        dc.SetBkMode(lmode);
        dc.SetTextColor(lcol);
    }

    void Viewer::OnFinalMessage(HWND)
    {
        delete this;
    }

    void Viewer::StartSearchInFile()
    {
        ::_NotImplementedYet(L"Viewer::StartSearchInFile");
    }
}
