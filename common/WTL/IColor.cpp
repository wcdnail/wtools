#include "stdafx.h"
#include "IColor.h"
#include "IColorObserver.h"
#include <dh.tracing.h>

IColor::~IColor() = default;

IColor::IColor()
    : m_pObserver{nullptr}
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

void IColor::SetObserver(IColorObserver& rObserver)
{
    m_pObserver = &rObserver;
}

IColorObserver* IColor::GetObserver() const
{
    return m_pObserver;
}

void IColor::NotifyObservers() const
{
    IColorObserver* pObserver = GetObserver();
    while (pObserver) {
        pObserver->OnColorUpdate(*this);
        pObserver = nullptr; //pObserver->NextObserver();
    }
}
