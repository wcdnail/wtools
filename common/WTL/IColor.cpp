#include "stdafx.h"
#include "IColor.h"
#include <dh.tracing.h>

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
}
