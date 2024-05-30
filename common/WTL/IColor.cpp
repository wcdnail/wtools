#include "stdafx.h"
#include "IColor.h"
#include <dh.tracing.h>

IColor::~IColor() = default;

COLORREF IColor::GetColorRef() const
{
    DH::TPrintf(L"%s: NOT IMPLEMENTED\n", __FUNCTIONW__);
    ATLASSERT(false);
    return CLR_INVALID;
}

int IColor::GetAlpha() const
{
    DH::TPrintf(L"%s: NOT IMPLEMENTED\n", __FUNCTIONW__);
    ATLASSERT(false);
    return -1;
}

void IColor::SetColor(COLORREF crColor, int nAlpha)
{
    UNREFERENCED_PARAMETER(crColor);
    UNREFERENCED_PARAMETER(nAlpha);
    DH::TPrintf(L"%s: NOT IMPLEMENTED\n", __FUNCTIONW__);
    ATLASSERT(false);
}

void IColor::SetColor(IColor const* pColor)
{
    if (!pColor || (pColor == this)) {
        return ;
    }
    SetColor(pColor->GetColorRef(), pColor->GetAlpha());
}
