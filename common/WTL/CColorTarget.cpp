#include "stdafx.h"
#include "CColorTarget.h"
#include <atlwin.h>

IColorTarget::~IColorTarget() = default;

void CColorTarget::OnUpdateColor(COLORREF crColor, int nAlpha) const
{
    if (m_pTarget) {
        m_pTarget->SetColor(crColor, nAlpha);
    }
}

void CColorTarget::UpdateHostColor() const
{
    if (m_pHost && m_pTarget) {
        m_pHost->SetColor(m_pTarget->GetColorRef(), m_pTarget->GetAlpha());
    }
}
