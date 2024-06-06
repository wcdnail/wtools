#include "stdafx.h"
#include "IColor.h"
#include "IColorObserver.h"
#include <dh.tracing.h>

IColor::~IColor() = default;
IColor::IColor()
    : m_nID{nInvalidID}
    , m_lstObservers{}
{
}

COLORREF IColor::GetColorRef() const
{
    DH::Printf(TL_Error, L"%s(%d): '%s' NOT IMPLEMENTED\n", __FILEW__, __LINE__, __FUNCTIONW__);
    ATLASSERT(false);
    return CLR_INVALID;
}

int IColor::GetAlpha() const
{
    DH::Printf(TL_Error, L"%s(%d): '%s' NOT IMPLEMENTED\n", __FILEW__, __LINE__, __FUNCTIONW__);
    ATLASSERT(false);
    return -1;
}

void IColor::SetColor(COLORREF crColor, int nAlpha)
{
    UNREFERENCED_PARAMETER(crColor);
    UNREFERENCED_PARAMETER(nAlpha);
    DH::Printf(TL_Error, L"%s(%d): '%s' NOT IMPLEMENTED\n", __FILEW__, __LINE__, __FUNCTIONW__);
    ATLASSERT(false);
}

void IColor::SetColor(IColor const* pColor)
{
    if (!pColor || (pColor == this)) {
        return ;
    }
    SetColor(pColor->GetColorRef(), pColor->GetAlpha());
    NotifyObservers();
}

void IColor::AddObserver(IColorObserver& rObserver)
{
    if (IsAlreadyObserved(&rObserver)) {
        return ;
    }
    m_lstObservers.push_back(&rObserver);
}

void IColor::AddObservers(IColor& rColor)
{
    ObserverList temp{};
    for (auto& it: rColor.m_lstObservers) {
        if (IsAlreadyObserved(it)) {
            continue;
        }
        temp.push_back(it);
    }
    m_lstObservers.assign_range(temp);
}

void IColor::RemoveObserver(IColorObserver& rObserver)
{
    auto const it = std::ranges::find(m_lstObservers, &rObserver);
    if (it == m_lstObservers.end()) {
        return ;
    }
    m_lstObservers.erase(it);
}

bool IColor::IsAlreadyObserved(IColorObserver const* pObserver) const
{
    auto const it = std::ranges::find(m_lstObservers, pObserver);
    return it != m_lstObservers.end();
}

void IColor::NotifyObservers() const
{
    for (const auto& it: m_lstObservers) {
        it->OnColorUpdate(*this);
    }
}
