#include "stdafx.h"
#include "IColorObserver.h"
#include <dh.tracing.h>

IColorObserver::~IColorObserver() = default;

IColorObserver::IColorObserver()
{
}

void IColorObserver::OnColorUpdate(IColor const& clrSource) 
{
    UNREFERENCED_PARAMETER(clrSource);
    DH::Printf(TL_Error, L"%s(%d): '%s' NOT IMPLEMENTED\n", __FILEW__, __LINE__, __FUNCTIONW__);
    ATLASSERT(false);
}
