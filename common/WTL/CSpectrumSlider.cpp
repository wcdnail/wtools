//
// https://www.codeproject.com/articles/8985/customizing-the-appearance-of-csliderctrl-using-cu
// Customizing the Appearance of CSliderCtrl Using Custom Draw
// Mike O'Neill
// adopted to WTL
//
#include "stdafx.h"
#include "CSpectrumSlider.h"
#include <DDraw.DGI/DDGDIStuff.h>

CSpectrumSlider::~CSpectrumSlider() = default;

CSpectrumSlider::CSpectrumSlider()
    :    m_bMsgHandled{FALSE}
    ,      m_crPrimary{0x00000000}
    ,       m_crShadow{0x00000000}
    ,       m_crHilite{0x00000000}
    ,    m_crMidShadow{0x00000000}
    , m_crDarkerShadow{0x00000000}
    ,    m_normalBrush{}
    ,     m_focusBrush{}
{
}

ATOM& CSpectrumSlider::GetWndClassAtomRef()
{
    static ATOM gs_CSpectrumSlider_Atom{0};
    return gs_CSpectrumSlider_Atom;
}

BOOL CSpectrumSlider::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL const bSave{m_bMsgHandled};
    BOOL const  bRet{_ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID)};
    m_bMsgHandled = bSave;
    return bRet;
}

BOOL CSpectrumSlider::_ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
    BOOL bHandled{TRUE};
    UNREFERENCED_PARAMETER(hWnd);
    switch(dwMsgMapID) { 
    case 0:
        REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_CUSTOMDRAW, OnCustomDraw)
        NOTIFY_CODE_HANDLER_EX(NM_CUSTOMDRAW, OnCustomDraw)
    ALT_MSG_MAP(1)
        REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_CUSTOMDRAW, OnCustomDraw)
        break;
    default:
        ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
        ATLASSERT(FALSE);
        break;
    }
    return FALSE;
}

int CSpectrumSlider::OnCreate(LPCREATESTRUCT pCS)
{
    ATLTRACE(ATL::atlTraceControls, 0, _T("WM_CREATE OK for %p\n"), this);
    return 0;
}

void CSpectrumSlider::SetPrimaryColor(COLORREF crPrimary)
{
    // sets primary color of control, and derives shadow and hilite colors
    // also initializes brushes that are used in custom draw functions
    m_crPrimary = crPrimary;

    // get hilite and shadow colors
    // uses the very-nice shell function of ColorAdjustLuma, described at
    // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/reference/shlwapi/gdi/coloradjustluma.asp
    // for which we need at least IE version 5.0 and above, and a link to shlwapi.lib
    m_crHilite       = ColorAdjustLuma(crPrimary,  500, TRUE); // increase by 50%
    m_crMidShadow    = ColorAdjustLuma(crPrimary, -210, TRUE); // decrease by 21.0%
    m_crShadow       = ColorAdjustLuma(crPrimary, -445, TRUE); // decrease by 44.5%
    m_crDarkerShadow = ColorAdjustLuma(crPrimary, -500, TRUE); // decrease by 50.0%

    // create normal (solid) brush 
    m_normalBrush.Attach(CreateSolidBrush(crPrimary));

    // create a hatch-patterned pixel pattern for patterned brush (used when thumb has focus/is selected)
    // see http://www.codeproject.com/gdi/custom_pattern_brush.asp
    // or look for BrushTool.exe for the code that generates these bits
    static const WORD bitsBrush1[8] = { 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa };
    WTL::CBitmap bm{};
    bm.CreateBitmap(8, 8, 1, 1, bitsBrush1);
    LOGBRUSH logBrush;
    logBrush.lbStyle = BS_PATTERN;
    logBrush.lbHatch = reinterpret_cast<ULONG_PTR>(bm.m_hBitmap);
    logBrush.lbColor = 0; // ignored anyway; must set DC background and text colors
    m_focusBrush.Attach(CreateBrushIndirect(&logBrush));
}

LRESULT CSpectrumSlider::OnCustomDraw(LPNMHDR pNMHDR) const
{
    // for additional info, read beginning MSDN "Customizing a Control's Appearance Using Custom Draw" at
    // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/custdraw/custdraw.asp
    // slider controls (which are actually called "trackbar" controls) are specifically discussed at
    // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/trackbar/notifications/nm_customdraw_trackbar.asp
    
    NMCUSTOMDRAW const& nmcd{*reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR)};
    UINT const     drawStage{nmcd.dwDrawStage};
    DWORD_PTR const itemSpec{nmcd.dwItemSpec};
    switch (drawStage) {
    // drawstage flags (for debugging purposes; copied from CommCtrl.h)
    // values under 0x00010000 are reserved for global custom draw values.
    // above that are for specific controls
    // #define CDDS_PREPAINT           0x00000001
    // #define CDDS_POSTPAINT          0x00000002
    // #define CDDS_PREERASE           0x00000003
    // #define CDDS_POSTERASE          0x00000004
    // the 0x000010000 bit means it's individual item specific
    // #define CDDS_ITEM               0x00010000
    // #define CDDS_ITEMPREPAINT       (CDDS_ITEM | CDDS_PREPAINT)
    // #define CDDS_ITEMPOSTPAINT      (CDDS_ITEM | CDDS_POSTPAINT)
    // #define CDDS_ITEMPREERASE       (CDDS_ITEM | CDDS_PREERASE)
    // #define CDDS_ITEMPOSTERASE      (CDDS_ITEM | CDDS_POSTERASE)
                
    // custom draw item specs (for debugging purposes; copied from CommCtrl.h)
    // #define TBCD_TICS    0x0001
    // #define TBCD_THUMB   0x0002
    // #define TBCD_CHANNEL 0x0003

    // itemState flags (for debugging purposes; copied from CommCtrl.h)
    //  #define CDIS_SELECTED       0x0001
    //  #define CDIS_GRAYED         0x0002
    //  #define CDIS_DISABLED       0x0004
    //  #define CDIS_CHECKED        0x0008
    //  #define CDIS_FOCUS          0x0010
    //  #define CDIS_DEFAULT        0x0020
    //  #define CDIS_HOT            0x0040
    //  #define CDIS_MARKED         0x0080
    //  #define CDIS_INDETERMINATE  0x0100

    case CDDS_PREPAINT: {
        // Before the paint cycle begins
        // most important of the drawing stages
        // must return CDRF_NOTIFYITEMDRAW or else we will not get further 
        // NM_CUSTOMDRAW notifications for this drawing cycle
        // we also return CDRF_NOTIFYPOSTPAINT so that we will get post-paint notifications
        WTL::CDCHandle     dc{nmcd.hdc};
        const int       iSave{dc.SaveDC()};
        const DWORD   dwStyle{GetStyle()};
        CRect const rcChannel{GetChannelRect()};
        CRect const   rcThumb{GetThumbRect()};
        CRect        rcClient{nmcd.rc};
        CRect           rcBar{};

        GetClientRect(&rcClient);
        // TBS_RIGHT, TBS_BOTTOM and TBS_HORZ are all defined as 0x0000, so avoid testing on them
        if (dwStyle & TBS_VERT) {
            rcBar.SetRect(rcThumb.right+4, rcChannel.left+rcThumb.Height()/2, rcClient.right-8, rcChannel.right-rcThumb.Height()/2);
        }
        else {
            // TODO: investigate it 
            rcBar.SetRect(rcClient.left+4, rcChannel.left+rcThumb.Height()/2, rcClient.right-8, rcChannel.right-rcThumb.Height()/2);
        }

        dc.FillSolidRect(rcBar, m_crPrimary);
        dc.RestoreDC(iSave);
        return CDRF_NOTIFYITEMDRAW | CDRF_SKIPDEFAULT;// | CDRF_NOTIFYPOSTPAINT;
    }
        
    case CDDS_PREERASE:         // Before the erase cycle begins
    case CDDS_POSTERASE:        // After the erase cycle is complete
    case CDDS_ITEMPREERASE:     // Before an item is erased
    case CDDS_ITEMPOSTERASE:    // After an item has been erased
        // these are not handled now, but you might like to do so in the future
        return CDRF_DODEFAULT;
        
    case CDDS_ITEMPREPAINT: {
        // Before an item is drawn
        // this is where we perform our item-specific custom drawing
        switch (itemSpec) {
        case TBCD_CHANNEL:  // channel that the trackbar control's thumb marker slides along
            // For the pre-item-paint of the channel, we simply tell the control to draw the default
            // and then tell us when it's done drawing the channel (i.e., item-post-paint) using
            // CDRF_NOTIFYPOSTPAINT.  In post-item-paint of the channel, we draw a simple 
            // colored highlight in the channel's recatngle

            // Frankly, when I returned CDRF_SKIPDEFAULT, in an attempt to skip drawing here
            // and draw everything in post-paint, the control seems to ignore the CDRF_SKIPDEFAULT flag,
            // and it seems to draw the channel even if we returned CDRF_SKIPDEFAULT
            return CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;

        case TBCD_TICS:     // the increment tick marks that appear along the edge of the trackbar control
                            // currently, there is no special drawing of the  tics
            return CDRF_DODEFAULT;
            
        case TBCD_THUMB: {
            // trackbar control's thumb marker. This is the portion of the control that the user moves
            // For the pre-item-paint of the thumb, we draw everything completely here, during item 
            // pre-paint, and then tell the control to skip default painting and NOT to notify 
            // us during post-paint.
            // If I asked for a post-paint notification, then for some reason, when the control gets to 
            // post-paint, it completely over-writes everthing that I do here, and draws the default thumb
            // (which is partially obscured by the thumb drawn here).  This happens even if in post-paint
            // I return another CDRF_SKIPDEFAULT.  I don't understand why.  
            // Anyway, it works fine if I draw everthing here, return CDRF_SKIPDEFAULT, and do not ask for
            // a post-paint item notification

            //WTL::CDCHandle dcThumb{nmcd.hdc};
            //const int      iSaveDC{dcThumb.SaveDC()};
            //const WTL::CBrush* pBr{&m_normalBrush};
            //const WTL::CPen    pen{CreatePen(PS_SOLID, 2, m_crShadow)};

            //// if thumb is selected/focussed, switch brushes
            //if (nmcd.uItemState && CDIS_FOCUS) {
            //    pBr = &m_focusBrush;
            //    dcThumb.SetBrushOrg(nmcd.rc.right % 8, nmcd.rc.top % 8);
            //    dcThumb.SetBkColor(m_crPrimary);
            //    dcThumb.SetTextColor(m_crHilite);                
            //}
            //dcThumb.SelectBrush(*pBr);
            //dcThumb.SelectPen(pen);

            //int const xx{nmcd.rc.left+3};
            //int const yy{nmcd.rc.top+1};
            //int const rx{nmcd.rc.right-8};
            //int const by{nmcd.rc.bottom-1};
            //int const cx{rx - xx};
            //int const cy{by - yy};
            //const POINT pts[]{{xx, yy}, {rx, yy}, {rx+cx/2, yy+cy/2}, {rx, by}, {xx, by}};
            //dcThumb.Polygon(pts, _countof(pts));
            //dcThumb.RestoreDC(iSaveDC);
            //return CDRF_SKIPDEFAULT;    // don't let control draw itself, or it will un-do our work
            return CDRF_DODEFAULT;
        }
        default:
            ATLASSERT(FALSE);           // all of a slider's items have been listed, so we shouldn't get here
        };
        break;
    }

    case CDDS_ITEMPOSTPAINT:    // After an item has been drawn
        switch (itemSpec) {
        case TBCD_CHANNEL: {
            // channel that the trackbar control's thumb marker slides along
            // For the item-post-paint of the channel, we basically like what the control has drawn, 
            // which is a four-line high rectangle whose colors (in order) are white, mid-gray, black, 
            // and dark-gray.
            // However, to emphasize the control's color, we will replace the middle two lines
            // (i.e., the mid-gray and black lines) with hilite and shadow colors of the control
            // using CDC::Draw3DRect.
            WTL::CDCHandle dcChannel{nmcd.hdc};
            CRect const       rcRect{nmcd.rc.left+1, nmcd.rc.top+1, nmcd.rc.right-1, nmcd.rc.bottom-1};
            dcChannel.Draw3dRect(rcRect, m_crMidShadow, m_crHilite);
            return CDRF_SKIPDEFAULT;
        }
        case TBCD_TICS:     // the increment tick marks that appear along the edge of the trackbar control
                            // currently, there is no special post-item-paint drawing of the tics
            return CDRF_DODEFAULT;
        case TBCD_THUMB:    // trackbar control's thumb marker. This is the portion of the control that the user moves
                            // currently, there is no special post-item-paint drawing for the thumb
            return CDRF_DODEFAULT;  // don't let control draw itself, or it will un-do our work
        default:
            ATLASSERT(FALSE); // all of a slider's items have been listed, so we shouldn't get here
        };
        break;

    case CDDS_POSTPAINT: {
        // After the paint cycle is complete
        // this is the post-paint for the entire control, and it's possible to add to whatever is 
        // now visible on the control
        // To emphasize the directivity of the control, we simply draw in two colored dots at the 
        // extreme edges of the control
        return CDRF_SKIPDEFAULT;
    }
    default:
        ATLASSERT(FALSE);   // all drawing stages are listed, so we shouldn't get here
    };
    return CDRF_DODEFAULT;
}
