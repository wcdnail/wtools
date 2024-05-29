#include "stdafx.h"
#include "CColorTarget.h"
#include <dh.tracing.h>

IColorTarget::~IColorTarget() = default;
IColorTarget::IColorTarget() = default;

COLORREF IColorTarget::GetColorRef() const
{
    DH::TPrintf(L"%s: NOT IMPLEMENTED\n", __FUNCTIONW__);
    ATLASSERT(false);
    return CLR_INVALID;
}

int IColorTarget::GetAlpha() const
{
    DH::TPrintf(L"%s: NOT IMPLEMENTED\n", __FUNCTIONW__);
    ATLASSERT(false);
    return -1;
}

void IColorTarget::SetColor(COLORREF crColor, int nAlpha)
{
    UNREFERENCED_PARAMETER(crColor);
    UNREFERENCED_PARAMETER(nAlpha);
    DH::TPrintf(L"%s: NOT IMPLEMENTED\n", __FUNCTIONW__);
    ATLASSERT(false);
}

void IColorTarget::SetColorTarget(IColorTarget& rTarget)
{
    SourceSetColor(rTarget);
}

void IColorTarget::SourceSetColor(IColorTarget const& trgSource)
{
    SetColor(trgSource.GetColorRef(), trgSource.GetAlpha());
}

void IColorTarget::TargetColorUpdate(COLORREF crColor, int nAlpha)
{
    UNREFERENCED_PARAMETER(crColor);
    UNREFERENCED_PARAMETER(nAlpha);
}
