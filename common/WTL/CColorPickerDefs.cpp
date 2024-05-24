#include "stdafx.h"
#include "CColorPickerDefs.h"
#include <DDraw.DGI/DDGDIStuff.h>
#include <dh.tracing.h>

IMPL_DEFAULT_COPY_MOVE_OF(CColorUnion);

CColorUnion::~CColorUnion() = default;

CColorUnion::CColorUnion(double dH, double dS, double dV)
    : m_bUp{false}
    ,   m_H{static_cast<int>(dH)}
    ,   m_S{static_cast<int>(dS)}
    ,   m_V{static_cast<int>(dV)}
    ,   m_R{0}
    ,   m_G{0}
    ,   m_B{0}
    ,   m_A{0xff}
    ,  m_Hl{0}
    ,  m_Sl{0}
    ,   m_L{0}
{
    HSVtoRGB();
}

CColorUnion::CColorUnion(COLORREF crInitial)
    : m_bUp{false}
    ,   m_H{0}
    ,   m_S{0}
    ,   m_V{0}
    ,   m_R{GetRValue(crInitial)}
    ,   m_G{GetGValue(crInitial)}
    ,   m_B{GetBValue(crInitial)}
    ,   m_A{0xff}
    ,  m_Hl{0}
    ,  m_Sl{0}
    ,   m_L{0}
{
    RGBtoHSV();
}

CRGBSpecRect CColorUnion::GetRGBSpectrumRect(SpectrumKind nSpectrumKind) const
{
    CRGBSpecRect rv{};
    switch (nSpectrumKind) {
    case SPEC_RGB_Red: {
        auto const R{GetRed()};
        rv.crLT = RGB(R, 0, 0);
        rv.crRT = RGB(R, 255, 0);
        rv.crLB = RGB(R, 0, 255);
        rv.crRB = RGB(R, 255, 255);
        break;
    }
    case SPEC_RGB_Green:{
        auto const G{GetGreen()};
        rv.crLT = RGB(0, G, 0);
        rv.crRT = RGB(255, G, 0);
        rv.crLB = RGB(0, G, 255);
        rv.crRB = RGB(255, G, 255);
        break;
    }
    case SPEC_RGB_Blue:{
        auto const B{GetBlue()};
        rv.crLT = RGB(0, 0, B);
        rv.crRT = RGB(0, 255, B);
        rv.crLB = RGB(255, 0, B);
        rv.crRB = RGB(255, 255, B);
        break;
    }
    default:
        ATLASSERT(FALSE);
        break;
    }
    return rv;
}

CPoint CColorUnion::GetColorPoint(SpectrumKind nSpectrumKind, CRect const& rc)
{
    double xScale{1.};
    double yScale{1.};
    CPoint     rv{0, 0};
    switch (nSpectrumKind) {
    case SPEC_RGB_Red:
    case SPEC_RGB_Green:
    case SPEC_RGB_Blue:
        xScale = static_cast<double>(rc.Width()) / RGB_MAX;
        yScale = static_cast<double>(rc.Height()) / RGB_MAX;
        switch (nSpectrumKind) {
        case SPEC_RGB_Red:   rv.SetPoint(static_cast<int>(GetGreen() * xScale), static_cast<int>(GetBlue() * yScale)); break;
        case SPEC_RGB_Green: rv.SetPoint(  static_cast<int>(GetRed() * xScale), static_cast<int>(GetBlue() * yScale)); break;
        case SPEC_RGB_Blue:  rv.SetPoint(static_cast<int>(GetGreen() * xScale),  static_cast<int>(GetRed() * yScale)); break;
        default:
            ATLASSERT(FALSE);
            break;
        }
        break;
    case SPEC_HSV_Hue:
        xScale = rc.Width() / HSV_100PERC;
        yScale = rc.Height() / HSV_100PERC;
        rv.SetPoint(static_cast<int>(m_S * xScale), static_cast<int>(rc.Height() - (m_V * yScale)));
        break;
    case SPEC_HSV_Saturation:
        xScale = rc.Width() / HSV_HUE_MAX;
        yScale = rc.Height() / HSV_100PERC;
        rv.SetPoint(static_cast<int>(m_H * xScale), static_cast<int>(rc.Height() - (m_V * yScale)));
        break;
    case SPEC_HSV_Brightness:
        xScale = rc.Width() / HSV_HUE_MAX;
        yScale = rc.Height() / HSV_100PERC;
        rv.SetPoint(static_cast<int>(m_H * xScale), static_cast<int>(rc.Height() - (m_S * yScale)));
        break;
    default:
        ATLASSERT(FALSE);
        break;
    }
    return rv;
}

PCTSTR SkipHexHead(PCTSTR pBeg, int& nLen, EHexHeadType& hType)
{
    if (nLen < 1) {
        return nullptr;
    }
    if (nLen >= 2) {
        if (0 == _tcsnicmp(pBeg, _T("0x"), 2)) {
            pBeg += 2;
            nLen -= 2;
            hType = HEX_STR_CPP;
        }
        else if (0 == _tcsnicmp(pBeg, _T("&H"), 2)) {
            pBeg += 2;
            nLen -= 1;
            hType = HEX_STR_ASS;
        }
        else if (pBeg[0] == _T('#')) {
            pBeg += 1;
            nLen -= 1;
            hType = HEX_STR_WEB;
        }
    }
    return pBeg;
}

bool CColorUnion::FromString(ATL::CString& sColor, bool bSwapRB)
{
    EHexHeadType hType{HEX_STR_PLAIN};
    int           nLen{sColor.GetLength()};
    PCTSTR const  pBeg{SkipHexHead(sColor.GetString(), nLen, hType)};
    if (!pBeg) {
        return false;
    }
    PTSTR pEnd{sColor.GetBuffer() + nLen};
    switch (hType) {
    case HEX_STR_ASS: bSwapRB = false; break;
    case HEX_STR_WEB: bSwapRB = true; break;
    default: break;
    }
    ULONG const nRes{_tcstoul(pBeg, &pEnd, 16)};
    m_R = GetRValue(nRes);
    m_G = GetGValue(nRes);
    m_B = GetBValue(nRes);
    if (bSwapRB) {
        std::swap(m_R, m_B);
    }
    return true;
}
