#include "stdafx.h"
#include "IColor.h"
#include "IColorObserver.h"
#include <dh.tracing.h>

IColor::~IColor() = default;

IColor::IColor()
    : m_lstObservers{}
{
}

COLORREF IColor::GetColorRef() const
{
    DH::Printf(L"%s(%d): '%s' NOT IMPLEMENTED\n", __FILEW__, __LINE__, __FUNCTIONW__);
    ATLASSERT(false);
    return CLR_INVALID;
}

int IColor::GetAlpha() const
{
    DH::Printf(L"%s(%d): '%s' NOT IMPLEMENTED\n", __FILEW__, __LINE__, __FUNCTIONW__);
    ATLASSERT(false);
    return -1;
}

void IColor::SetColor(COLORREF crColor, int nAlpha)
{
    UNREFERENCED_PARAMETER(crColor);
    UNREFERENCED_PARAMETER(nAlpha);
    DH::Printf(L"%s(%d): '%s' NOT IMPLEMENTED\n", __FILEW__, __LINE__, __FUNCTIONW__);
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

bool IColor::IsAlreadyObserved(IColorObserver const* pObserver) const
{
    for (auto const& jt: m_lstObservers) {
        if (pObserver == jt) {
            return true;
        }
    }
    return false;
}

void IColor::NotifyObservers() const
{
    for (const auto& it: m_lstObservers) {
        it->OnColorUpdate(*this);
    }
}