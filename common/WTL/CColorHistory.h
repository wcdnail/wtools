#pragma once

#include <wcdafx.api.h>
#include <atltypes.h>
#include <atlgdi.h>
#include <deque>

#include "color.stuff.h"

struct CColorHistoryCell;

using CHistoryStore = std::deque<CColorHistoryCell>;

struct CColorHistoryCell
{
    COLORREF crColor;
    int       nAlpha;

    DELETE_COPY_OF(CColorHistoryCell);
    ~CColorHistoryCell();
    CColorHistoryCell(COLORREF crClr, int nA);

    CColorHistoryCell(CColorHistoryCell&& rhs) noexcept;
    CColorHistoryCell& operator = (CColorHistoryCell&& rhs) noexcept;
    void Swap(CColorHistoryCell& rhs) noexcept;

    HDC GetDC(HDC dc, CRect const& rc);
    bool operator == (const CColorHistoryCell&) const;

    void Draw(WTL::CDCHandle dc, CRect const& rc, HBRUSH brBack);

private:
    HBITMAP       m_hPrev;
    WTL::CDC         m_DC;
    WTL::CBitmap m_Bitmap;
};

struct CColorHistory
{
    DELETE_COPY_MOVE_OF(CColorHistory);
    ~CColorHistory();
    CColorHistory();

    bool Initialize(ATL::CWindow stHistory);

    bool PutFront(COLORREF crColor, int nAlpha);
    CColorHistoryCell const& Pick();
    void LoadColorTable(CF::ColorTabItem const* crTab, size_t nCount);
    void Draw(LPDRAWITEMSTRUCT pDI, HBRUSH brBack);

private:
    ATL::CWindow   m_stHistory;
    CHistoryStore m_deqHistory;

    static CColorHistoryCell const& DummyColorCell();

    size_t GetMaxLimit() const;
    bool DropTail();
};
