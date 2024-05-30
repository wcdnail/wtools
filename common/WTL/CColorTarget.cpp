#include "stdafx.h"
#include "CColorTarget.h"
#include "IColor.h"
#include <dh.tracing.h>
#include <scoped.bool.guard.h>

#if 0
IColorTarget::~IColorTarget() = default;
IColorTarget::IColorTarget()
    : m_pColorTarget{nullptr}
    ,   m_pColorHost{nullptr}
    ,    m_bSyncLoop{false}
{
}

void IColorTarget::SyncTargets(IColorTarget* pCurrent, COLORREF crColor, int nAlpha, bool& bInLoop) 
{
    CScopedBoolGuard guard{bInLoop};
    while (pCurrent) {
        pCurrent->SetColor(crColor, nAlpha);
        pCurrent = pCurrent->m_pColorTarget;
    }
}

void IColorTarget::SyncHosts(IColorTarget* pCurrent, COLORREF crColor, int nAlpha, bool& bInLoop)
{
    CScopedBoolGuard guard{bInLoop};
    while (pCurrent) {
        pCurrent->SetColor(crColor, nAlpha);
        pCurrent = pCurrent->m_pColorHost;
    }
}

void IColorTarget::SyncTargets(IColorTarget* pSource)
{
    if (!pSource || m_bSyncLoop) {
        return ;
    }
    COLORREF const crColor{GetColorRef()};
    int const       nAlpha{GetAlpha()};
    SyncTargets(pSource, crColor, nAlpha, m_bSyncLoop);
}

void IColorTarget::SyncHosts(IColorTarget* pSource)
{
    if (!pSource || m_bSyncLoop) {
        return ;
    }
    COLORREF const crColor{GetColorRef()};
    int const       nAlpha{GetAlpha()};
    SyncHosts(pSource, crColor, nAlpha, m_bSyncLoop);
}

void IColorTarget::SetColorTarget(IColorTarget& rTarget)
{
    rTarget.m_pColorHost = this;
    m_pColorTarget = &rTarget;
    SyncHosts(m_pColorTarget, rTarget.GetColorRef(), rTarget.GetAlpha(), m_bSyncLoop);
}
#endif

CColorTarget::~CColorTarget() = default;

CColorTarget::CColorTarget()
    : m_pTarget{nullptr}
{
}

void CColorTarget::SetTarget(IColor& clTarget)
{
    m_pTarget = &clTarget;
}

void CColorTarget::Update(IColor const& clrSource) const
{
    if (!m_pTarget) {
        return ;
    }
    m_pTarget->SetColor(&clrSource);
}
