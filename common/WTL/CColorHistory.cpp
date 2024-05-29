#include "stdafx.h"
#include "CColorHistory.h"
#include <color.stuff.h>
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

CColorHistoryCell::~CColorHistoryCell()
{
    if (m_hPrev) {
        m_DC.SelectBitmap(m_hPrev);
    }
}

CColorHistoryCell::CColorHistoryCell(COLORREF crClr, int nA)
    : crColor{crClr}
    ,  nAlpha{nA}
    , m_hPrev{nullptr}
{
}

CColorHistoryCell::CColorHistoryCell(CColorHistoryCell&& rhs) noexcept
    : crColor{rhs.crColor}
    ,  nAlpha{rhs.nAlpha}
    , m_hPrev{nullptr}
{
}

CColorHistoryCell& CColorHistoryCell::operator=(CColorHistoryCell&& rhs) noexcept
{
    rhs.Swap(*this);
    return *this;
}

void CColorHistoryCell::Swap(CColorHistoryCell& rhs) noexcept
{
    std::swap(crColor, rhs.crColor);
    std::swap(nAlpha, rhs.nAlpha);
    std::swap(m_hPrev, rhs.m_hPrev);
    std::swap(m_DC.m_hDC, rhs.m_DC.m_hDC);
    std::swap(m_Bitmap.m_hBitmap, rhs.m_Bitmap.m_hBitmap);
}

HDC CColorHistoryCell::GetDC(HDC dc, CRect const& rc)
{
    if (!m_DC) {
        if (!m_Bitmap) {
            m_Bitmap = CreateCompatibleBitmap(dc, rc.Width(), rc.Height());
        }
        m_DC.CreateCompatibleDC(dc);
        m_hPrev = m_DC.SelectBitmap(m_Bitmap);
        m_DC.FillSolidRect(rc, crColor);
    }
    return m_DC.m_hDC;
}

bool CColorHistoryCell::operator==(const CColorHistoryCell& lhs) const
{
    return nAlpha == lhs.nAlpha &&
          crColor == lhs.crColor;
}

void CColorHistoryCell::Draw(WTL::CDCHandle dc, CRect const& rc, HBRUSH brBack)
{
    if (nAlpha >= RGB_MAX_INT) {
        dc.FillSolidRect(rc, crColor);
        return ;
    }
    CRect const        rcColor{0, 0, 1, 1};
    WTL::CDCHandle const dcClr{GetDC(dc, rcColor)};
    BLENDFUNCTION const  blend{AC_SRC_OVER, 0, static_cast<BYTE>(nAlpha), 0};
    dc.SelectBrush(brBack);
    dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);
    dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(),
                  dcClr, 0, 0, rcColor.Width(), rcColor.Height(), blend);
}

CColorHistory::~CColorHistory() = default;

CColorHistory::CColorHistory()
    : m_stHistory{nullptr}
{
    UNREFERENCED_PARAMETER(StaticInit::Instance(*this));
}

bool CColorHistory::Initialize(ATL::CWindow stHistory)
{
    m_stHistory.Attach(stHistory);
    return true;
}

size_t CColorHistory::GetMaxLimit() const
{
    if (!m_stHistory.m_hWnd) {
        return 0;
    }
    CRect rc{};
    m_stHistory.GetClientRect(rc);
    auto const xCount{static_cast<size_t>((rc.Width() - 1) / HC_RADIUS) + 1};
    auto const yCount{static_cast<size_t>((rc.Height() - 1) / HC_RADIUS) + 1};
    return xCount * yCount;
}

bool CColorHistory::DropTail()
{
    if (!m_stHistory.m_hWnd) {
        return false;
    }
    auto const nHistMax{GetMaxLimit()};
    if (m_deqHistory.size() > nHistMax) {
        auto const pos{std::next(m_deqHistory.begin(), nHistMax)};
        m_deqHistory.erase(pos, m_deqHistory.end());
        return true;
    }
    return false;
}

bool CColorHistory::PutFront(COLORREF crColor, int nAlpha)
{
    CColorHistoryCell it{crColor, nAlpha};
    if (!m_deqHistory.empty() && it == m_deqHistory.front()) {
        return false;
    }
    m_deqHistory.emplace_front(std::move(it));
    m_stHistory.InvalidateRect(nullptr, FALSE);
    DropTail();
    return true;
}

CColorHistoryCell const& CColorHistory::Pick()
{
    if (!m_stHistory.m_hWnd) {
        return DummyColorCell();
    }
    CRect  rc{};
    CPoint pt{};
    GetCursorPos(&pt);
    m_stHistory.GetClientRect(rc);
    m_stHistory.ScreenToClient(&pt);
    auto const   xPos{static_cast<size_t>(pt.x / HC_RADIUS)};
    auto const   yPos{static_cast<size_t>(pt.y / HC_RADIUS)};
    auto const xCount{static_cast<size_t>((rc.Width() - 1) / HC_RADIUS) + 1};
    auto const nIndex{xPos + yPos * xCount};
    if (nIndex < m_deqHistory.size()) {
        return m_deqHistory[nIndex];
    }
    return DummyColorCell();
}

CColorHistoryCell const& CColorHistory::DummyColorCell()
{
    static CColorHistoryCell const dummy{0xff00ff, 255};
    return dummy;
}

void CColorHistory::LoadColorTable(CF::ColorTabItem const* crTab, size_t nCount)
{
    if (nCount < 1 || !crTab) {
        return ;
    }
    CHistoryStore deqTemp;
    for (size_t i = 0; i < nCount; i++) {
        CColorHistoryCell it{crTab[i].clrColor, RGB_MAX_INT};
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
