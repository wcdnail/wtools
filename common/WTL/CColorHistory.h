#pragma once

#include "CColorCell.h"
#include <wcdafx.api.h>
#include <deque>

namespace CF { struct ColorTabItem; }

struct CColorHistory
{
    using CHistoryStore = std::deque<CColorCell>;
    
    DELETE_COPY_MOVE_OF(CColorHistory);
    ~CColorHistory();
    CColorHistory();

    bool PutFront(COLORREF crColor, int nAlpha, ATL::CWindow stHistory);
    CColorCell const& Pick(ATL::CWindow stHistory);
    void LoadColorTable(CF::ColorTabItem const* crTab, size_t nCount);
    void Draw(LPDRAWITEMSTRUCT pDI, HBRUSH brBack);

private:
    struct StaticInit;

    CHistoryStore m_deqHistory;

    static CColorCell const& DummyColorCell();

    static size_t GetMaxLimit(ATL::CWindow stHistory);
    bool DropTail(ATL::CWindow stHistory);
};
