#pragma once

#include "CColorCell.h"
#include <wcdafx.api.h>
#include <deque>
#include <list>

namespace CF { struct ColorTabItem; }

struct CColorHistory
{
    using CHistoryStore = std::deque<CColorCell>;
    
    DELETE_COPY_MOVE_OF(CColorHistory);
    ~CColorHistory();
    CColorHistory();

    bool PutFront(COLORREF crColor, int nAlpha, ATL::CWindow stHistory);
    CColorCell const& Pick(ATL::CWindow stHistory);
    void Draw(LPDRAWITEMSTRUCT pDI, HBRUSH brBack);

    void AddHostClt(ATL::CWindow const& stHistory);
    void RemoveHostClt(ATL::CWindow const& stHistory);

private:
    struct StaticInit;

    using CHostCltList = std::list<ATL::CWindow>;

    CHistoryStore m_deqHistory;
    CHostCltList m_lstHostCtls;

    static CColorCell const& DummyColorCell();

    void LoadColorTable(CF::ColorTabItem const* crTab, size_t nCount);

    static size_t GetMaxLimit(ATL::CWindow stHistory);
    bool DropTail(ATL::CWindow stHistory);

    bool IsAlreadyHosted(ATL::CWindow const& stHistory) const;
};
