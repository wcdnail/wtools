#include "stdafx.h"
#include "CColorHistory.h"
#include <color.stuff.h>
#include <atltypes.h>
#include <atlmisc.h>

struct CColorHistory::StaticInit
{
    static StaticInit& Instance(CColorHistory& clrHistory)
    {
        static StaticInit inst{clrHistory};
        return inst;
    }

private:
    ~StaticInit() = default;
    StaticInit(CColorHistory& clrHistory)
    {
        clrHistory.LoadColorTable(CF::CLR_PALETTE0, CF::CLR_PALETTE0_COUNT);
    }
};

namespace
{
    enum Consts: int
    {
        HC_RADIUS = 18, // CHECKERS_CX
    };
}

CColorHistory::~CColorHistory() = default;

CColorHistory::CColorHistory()
{
    UNREFERENCED_PARAMETER(StaticInit::Instance(*this));
}

size_t CColorHistory::GetMaxLimit(ATL::CWindow stHistory)
{
    if (!stHistory.m_hWnd) {
        return 0;
    }
    CRect rc{};
    stHistory.GetClientRect(rc);
    auto const xCount{static_cast<size_t>((rc.Width() - 1) / HC_RADIUS) + 1};
    auto const yCount{static_cast<size_t>((rc.Height() - 1) / HC_RADIUS) + 1};
    return xCount * yCount;
}

bool CColorHistory::DropTail(ATL::CWindow stHistory)
{
    if (!stHistory.m_hWnd) {
        return false;
    }
    auto const nHistMax{GetMaxLimit(stHistory)};
    if (m_deqHistory.size() > nHistMax) {
        auto const pos{std::next(m_deqHistory.begin(), nHistMax)};
        m_deqHistory.erase(pos, m_deqHistory.end());
        return true;
    }
    return false;
}

bool CColorHistory::PutFront(COLORREF crColor, int nAlpha, ATL::CWindow stHistory)
{
    CColorCell it{crColor, nAlpha};
    if (!m_deqHistory.empty() && it == m_deqHistory.front()) {
        return false;
    }
    m_deqHistory.emplace_front(std::move(it));
    stHistory.InvalidateRect(nullptr, FALSE);
    DropTail(stHistory);
    return true;
}

CColorCell const& CColorHistory::Pick(ATL::CWindow stHistory)
{
    if (!stHistory.m_hWnd) {
        return DummyColorCell();
    }
    CRect  rc{};
    CPoint pt{};
    GetCursorPos(&pt);
    stHistory.GetClientRect(rc);
    stHistory.ScreenToClient(&pt);
    auto const   xPos{static_cast<size_t>(pt.x / HC_RADIUS)};
    auto const   yPos{static_cast<size_t>(pt.y / HC_RADIUS)};
    auto const xCount{static_cast<size_t>((rc.Width() - 1) / HC_RADIUS) + 1};
    auto const nIndex{xPos + yPos * xCount};
    if (nIndex < m_deqHistory.size()) {
        return m_deqHistory[nIndex];
    }
    return DummyColorCell();
}

CColorCell const& CColorHistory::DummyColorCell()
{
    static CColorCell const dummy{0xff00ff, 255};
    return dummy;
}

void CColorHistory::LoadColorTable(CF::ColorTabItem const* crTab, size_t nCount)
{
    if (nCount < 1 || !crTab) {
        return ;
    }
    CHistoryStore deqTemp;
    for (size_t i = 0; i < nCount; i++) {
        CColorCell it{crTab[i].clrColor, RGB_MAX_INT};
        deqTemp.emplace_front(std::move(it));
    }
    deqTemp.swap(m_deqHistory);
}

void CColorHistory::Draw(LPDRAWITEMSTRUCT pDI, HBRUSH brBack)
{
    WTL::CDCHandle  dc{pDI->hDC};
    CRect const     rc{pDI->rcItem};
    int const    iSave{dc.SaveDC()};
    HBRUSH const brBrd{WTL::AtlGetStockBrush(WHITE_BRUSH)};
    CRect       rcItem{0, 0, HC_RADIUS, HC_RADIUS};
    int           nTop{0};
    for (auto& it: m_deqHistory) {
        it.Draw(dc, rcItem, brBack);
        dc.FrameRect(rcItem, brBrd);
        rcItem.left += HC_RADIUS;
        rcItem.right = rcItem.left + HC_RADIUS;
        if (rcItem.left >= rc.right) {
            nTop += rcItem.Height();
            rcItem.SetRect(0, nTop, HC_RADIUS, nTop + HC_RADIUS);
            if (rcItem.top >= rc.bottom) {
                break;
            }
        }
    }
    dc.RestoreDC(iSave);
}
