#pragma once

#include "wcdafx.api.h"
#include <atltypes.h>
#include <atlstr.h>
#include <atlcrack.h>
#include <atlgdi.h>
#include <atlctrls.h>
#include <map>

namespace Cf
{
    class Colorizer
    {
        Colorizer(Colorizer const&) = delete;
        Colorizer& operator = (Colorizer const&) = delete;

    protected:
        WCDAFX_API Colorizer();
        WCDAFX_API ~Colorizer();

    protected:
        WCDAFX_API BOOL ProcessWindowMessage(HWND, UINT, WPARAM, LPARAM, LRESULT&, DWORD = 0);

    protected:
        struct PaintContext;
        struct NcPainContext;

        struct ControlBase;
        typedef std::shared_ptr<ControlBase> ControlBasePtr;
        template <typename T> struct Control;
        template <typename U> struct Details;
        template <typename U> struct SpecificMembers;

        typedef ControlBase* (*ControlCreator)(HWND, Colorizer*);
        typedef std::map<CStringW, ControlCreator> FactoryMap;
        typedef std::map<HWND, ControlBasePtr> ControlMap;

        enum : uint64_t
        {
            MESSAGE_HANDLED = 0x0000000000000001
        };

        mutable uint64_t         Flags;
        HWND                    MyHwnd;
        ControlMap            Controls;
        COLORREF           MyTextColor;
        COLORREF           MyBackColor;
        COLORREF        MyHotTextColor;
        COLORREF        MyHotBackColor;
        WTL::CPen                MyPen;
        WTL::CPen           MyFocusPen;
        WTL::CPen       MyThickEdgePen;
        COLORREF     MyButtonBackColor[2];
        WTL::CBrush        MyBackBrush[3];
        WTL::CPen          MyBorderPen[2];

        static WTL::CImageList SpecGxf;

        enum SpecGfxIndex: UINT
        {
            SpecgSmallDown = 0,
            SpecgSmellRight,
            SpecgChecked,
            SpecgMinimize,
            SpecgRestore,
            SpecgClose,
            SpecgMaximize,
            SpecgSmallUp,
            SpecgFollowDown,
            SpecgSmallLeft,
            SpecgSmallCross,
            SpecgFollowRight,
            SpecgRound,
            SpecgDown,
            SpecgRight,
            SpecgPin,
            SpecgPinned,
            SpecgLeft,
            SpecgBackward,
            SpecgForward,
            SpecgBlackRight,
            SpecgBlackLeft,
            SpecgWhiteRight,
            SpecgWhiteLeft,
            SpecgUp,
            SpecgToLeft,
            SpecgToRight,
            SpecgArrowLeft,
            SpecgArrowRight,
            SpecgDropDown,
            SpecgDropRight,
            SpecgFfRight,
            SpecgFfDown,
            SpecgBlockDown,
            SpecgBlackCross,
            SpecgToOther,
        };

    private:
        enum BorderFlags : UINT
        {
            BorderNone = 0,
            BorderFrame,
            BorderSunken,
            BorderStaticEdge,
            BorderClientEdge
        };

        static FactoryMap Factory;
        static void PerformInitStatix();

        template <typename T>
        static ControlBase* Creator(HWND hwnd, Colorizer* owner);

        template <typename T>
        static void InsertToFactory();

        bool IsMsgHandled() const;
        void SetMsgHandled(bool nv) const;
        BOOL _ProcessWindowMessage(HWND, UINT, WPARAM, LPARAM, LRESULT&, DWORD);

        int OnCreate(LPCREATESTRUCT);
        BOOL OnInitDialog(HWND, LPARAM);
        BOOL OnEraseBkgnd(CDCHandle dc) const;
        void OnNcPaint(CRgnHandle rgn) const;
        void OnDestroy();
        HBRUSH OnCtlColorStatic(CDCHandle dc, HWND);
        LRESULT OnDrawItem(UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT OnNotify(UINT message, WPARAM wParam, LPARAM lParam);
      
        void SetTextColor(CDCHandle dc) const;

        void SuperclassEachChild(HWND hwnd);
        int DoInitialization(bool isDialog);
        static BOOL CALLBACK InitChild(HWND hwnd, Colorizer* self);

        static BorderFlags GetBorderType(HWND hwnd, LONG& style, LONG& estyle);
        void DrawControlBorder(CDCHandle dc, CRect const& rcPaint, BorderFlags border) const;

        template <typename T>
        static void PutText(Control<T>& child, CDCHandle dc, CRect const& rc, CString const& text, bool deflate = false, int dx = 0, int dy = 0);

        template <typename T>
        void OnEraseBackground(Control<T>& child, CDCHandle dc) const;
        
        void DrawGroupBox(Control<WTL::CButton>& child, CDCHandle dc, CRect const& rc, CString const& text) const;
        void DrawButton(Control<WTL::CButton>& child, CDCHandle dc, CRect const& rc, CString const& text, bool flat, bool _default) const;
        void DrawCheckBox(Control<WTL::CButton>& child, CDCHandle dc, CRect const& rc, CString const& text) const;
        void DrawRadioButton(Control<WTL::CButton>& child, CDCHandle dc, CRect const& rc, CString const& text) const;

        template <typename T>
        void DrawItem(Control<T>& child, int id, LPDRAWITEMSTRUCT di, bool eraseback = true) const;

        template <typename T>
        void DrawComboFace(Control<T>& child, CDCHandle dc, CRect const& rc) const;
    };
}