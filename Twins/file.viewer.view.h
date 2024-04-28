#pragma once

#include "file.viewer.types.h"
#include "file.viewer.buffer.h"
#include "strint.h"
#include <atlwin.h>
#include <atltypes.h>
#include <atlstr.h>
#include <atlctrls.h>

namespace Fl
{
    class Entry;
}

namespace Fv
{
    class Viewer: ATL::CWindowImpl<Viewer>
                , WTL::CDialogResize<Viewer>
    {
    private:
        typedef ATL::CWindowImpl<Viewer> Super;
        typedef WTL::CDialogResize<Viewer> Resizer;

    public:
        enum DisplayModes
        {
            PlainText = 0,
            Binary,
            Hex,
            Media,
            MaxMode
        };

        Viewer();
        ~Viewer();

        ErrorCode Create(HWND parent, CRect& rc, UINT id = 0);
        ErrorCode Open(Fl::Entry const& entry, SizeType bs = 1024 * 1024);
        ErrorCode Open(Path const& path, SizeType bs = 1024 * 1024);
        CIcon const& GetBigIcon() const;
        CIcon const& GetSmallIcon() const;
        Path const& GetPath() const;

        using Super::operator HWND;
        using Super::SetFocus;
        using Super::DestroyWindow;

    private:
        friend class Super;
        friend class Resizer;

        enum
        {
            ID_VSCROLL = 1000,
            ID_HSCROLL,
        };

        BEGIN_MSG_MAP_EX(Viewer)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_TIMER(OnTimer)
            MSG_WM_KEYDOWN(OnKeyDown)
            MSG_WM_MOUSEWHEEL(OnMouseWheel)
            CHAIN_MSG_MAP(Resizer)
            DEFAULT_REFLECTION_HANDLER()
        END_MSG_MAP()

        BEGIN_DLGRESIZE_MAP(Frame)
            DLGRESIZE_CONTROL(ID_VSCROLL, DLSZ_MOVE_X | DLSZ_SIZE_Y)
        END_DLGRESIZE_MAP()

    private:
        struct Fonts
        {
            CFont Default;
            CFont Terminal;
            CFont Status;
            Fonts();
        };

        struct Colors
        {
            COLORREF Background;
            COLORREF BackgroundLineNumber;
            COLORREF BackgroundStatus;
            COLORREF Text;
            COLORREF TextLineNumber;
            COLORREF TextStatusLine;
            COLORREF TextStatusMemory;
            COLORREF TextHexOffset;
            COLORREF BackgroundHexOffset;
            COLORREF TextHexBytes;
            COLORREF TextHexSymbols;
            COLORREF BackgroundHexSymbols;
            Colors();
            void LoadSystemValues();
        };

        struct DisplayState
        {
            FileBuffer InBuffer;
            unsigned CodePage;
            int LineCount;
            SizeType StartOffset;
            SizeType DisplayOffset;
            int VisibleLineLen;
            int FixedLineWidth;
            SizeType WrappedLineWidth;
            int HexLineWidth;
            bool ShowSpecChars;
            TEXTMETRIC TextMetric;

            DisplayState();
        };

        typedef void (*DisplayLineFtor)(CDCHandle, CRect const&, DisplayState&, Colors const&);
        typedef SizeType (*ScrollFtor)(DisplayState&, int);
        typedef SizeType (*Go2EndFtor)(DisplayState&);

        mutable DisplayState State;
        Colors Color;
        mutable Fonts Font;
        int DisplayMode;
        bool ShowStatusBar;
        int MiceScrollAmount;
        WString InputStatus;
        WString MemoryStatus;
        mutable WString DebugStatus;
        mutable CRect StatusRect;
        CScrollBar VScroller;
        CIcon IconBig;
        CIcon Icon;

        static wchar_t DisplayModeChar[MaxMode];
        static DisplayLineFtor DisplayLine[MaxMode];
        static ScrollFtor ScrollTo[MaxMode];
        static Go2EndFtor GoToEnd[MaxMode];

        static void DisplayTextLine(CDCHandle, CRect const&, DisplayState& s, Colors const& c);
        static void DisplayBinaryLine(CDCHandle, CRect const&, DisplayState& s, Colors const& c);
        static void DisplayHexLine(CDCHandle, CRect const&, DisplayState& s, Colors const& c);
        static SizeType ScrollFixed(DisplayState& s, int amount, int stride);
        static SizeType ScrollInText(DisplayState& s, int amount);
        static SizeType ScrollInBinary(DisplayState& s, int amount);
        static SizeType ScrollInHex(DisplayState& s, int amount);
        static SizeType Go2EndFixed(DisplayState& s, int stride);
        static SizeType Go2EndInText(DisplayState& s);
        static SizeType Go2EndInBinary(DisplayState& s);
        static SizeType Go2EndInHex(DisplayState& s);

        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        BOOL OnEraseBkgnd(CDCHandle) const;
        void OnPaint(CDCHandle) const;
        void OnTimer(UINT_PTR id);
        void DrawStatusLine(CDCHandle dc, CRect const& rcView, CRect const& rc) const;
        void InvalidateStatus();
        void DrawLine(CDCHandle dc, CRect& rcLine) const;
        void Reset(bool invalidate);
        void OnKeyDown(UINT code, UINT, UINT flags);
        BOOL OnMouseWheel(UINT flags, short delta, CPoint point);
        CRect GetViewRect(CRect const& rc) const;
        bool IsLineWrapOn() const;
        void ToggleLineWrap(bool invalidate = true);
        void ToggleShowSpec(bool invalidate = true);
        void Scroll(int amount, bool invalidate = true);
        void Go2Begin(bool invalidate = true);
        void Go2End(bool invalidate = true);
        void SetDisplayMode(int mode, bool invalidate = true);
        void AdjustFontSize(int dy, bool invalidate = true);
        CFont& GetDisplayFont() const;
        virtual void OnFinalMessage(HWND);
        void RedirectMessageToParent(bool async) const;
        void StartSearchInFile();
    };
}
