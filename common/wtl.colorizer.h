#pragma once

#include "wcdafx.api.h"
#include "strint.h"
#include <atltypes.h>
#include <atlstr.h>
#include <atlgdi.h>
#include <memory>
#include <unordered_map>
#include <map>

#include "wtl.control.h"

namespace CF::Colorized
{
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

    enum BorderFlags : UINT
    {
        BorderNone = 0,
        BorderFrame,
        BorderSunken,
        BorderStaticEdge,
        BorderClientEdge
    };

    class Colorizer;
    struct PaintContext;
    struct NcPainContext;
    struct IControl;
    template <typename T> struct Control;
    template <typename U> struct Details;
    template <typename U> struct SpecImpl;

    using ControlMap = std::map<HWND, ControlPtr>;

    WCDAFX_API WTL::CImageList& SpecGxf();

    class Colorizer: public ControlBase
    {
    public:
        Colorizer(Colorizer const&) = delete;
        Colorizer& operator = (Colorizer const&) = delete;

        WCDAFX_API ~Colorizer() override;
        WCDAFX_API Colorizer();

    protected:
        ControlMap Controls;

        WCDAFX_API BOOL OnWindowMessage(HWND, UINT, WPARAM, LPARAM, LRESULT&, DWORD) override;

    private:
        static void PerformInitStatix();

        int OnCreate(LPCREATESTRUCT);
        BOOL OnInitDialog(HWND, LPARAM);
        BOOL OnEraseMyBkgnd(CDCHandle dc);
        void OnNcPaint(CRgnHandle rgn);
        void OnDestroy();
        HBRUSH OnCtlColorStatic(CDCHandle dc, HWND);
        LRESULT OnDrawItem(UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT OnNotify(UINT message, WPARAM wParam, LPARAM lParam);
      
        void SuperclassForEachChild(HWND hwnd);
        int DoInitialization(bool isDialog);
        static BOOL CALLBACK InitChild(HWND hwnd, Colorizer& self);

    public:
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

        WCDAFX_API BOOL ProcessColorizerMessage(HWND, UINT, WPARAM, LPARAM, LRESULT&, DWORD = 0);

        WCDAFX_API void SetTextColor(CDCHandle dc) const;

        WCDAFX_API static BorderFlags GetBorderType(HWND hwnd, LONG& style, LONG& estyle);
        WCDAFX_API void DrawControlBorder(CDCHandle dc, CRect const& rcPaint, BorderFlags border) const;

        WCDAFX_API void OnEraseBackground(CDCHandle dc, CRect const& rc);
        WCDAFX_API void DrawGroupBox(CDCHandle dc, CRect const& rc, CString const& text, LONG style) const;
        WCDAFX_API void DrawCheckBox(CDCHandle dc, CRect const& rc, CString const& text, UINT tformat, LONG style, LONG estyle, UINT state) const;
        WCDAFX_API void DrawRadioButton(CDCHandle dc, CRect const& rc, CString const& text, UINT tformat, LONG style, LONG estyle, UINT state) const;
        WCDAFX_API void DrawComboFace(CDCHandle dc, CRect const& rc, int iid, CStringW const& text, CImageList const& ilist, int iindex) const;

        WCDAFX_API static void PutText(CDCHandle dc, const CRect& rc, const CString& text, UINT tformat, bool deflate = false, int dx = 0, int dy = 0);

        WCDAFX_API void DrawButton(CDCHandle dc,
                        const CRect&         rc,
                        const CString&     text,
                        UINT            tformat /* = Details<T>::GetDrawTextFormat(citem)*/,
                        UINT              state,
                        bool               flat,
                        bool           _default) const;

        WCDAFX_API void DrawItem(LPDRAWITEMSTRUCT  di,
                      const CString&  text /* citem.GetItemText(di->itemID) */,
                      CImageList     ilist /* citem.GetImageList()*/,
                      int           iindex /* citem.GetImageIndex(di->itemID)  */,
                      bool       eraseback = true) const;
    };
}
