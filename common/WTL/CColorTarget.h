#pragma once

#include "IColor.h"
#include <wcdafx.api.h>

struct CColorTarget
{
    ~CColorTarget();
    CColorTarget(IColor& clrMaster);

    void SetTarget(IColor& clrTarget);
    void Update(IColor& clrSource) const;

private:
    IColor* m_pMaster;
    IColor* m_pTarget;
};

#if 0
/**
 *
 * Linked targets example
 *
 *                                                     ┌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌◁ nullptr
 *          (nullptr == target)                        ┊                          ▲
 *                  (2)                                ┊           ┏━━━<target>━━━┛   (1.3)
 *                                                     ▽           ┃
 *                  (2.1)                        ┏━━━<host>━━◀ CColorCell ◀━━━<color>
 *                                               ┃                 ▲
 *                                               ┃  ┏━━━<target>━━━┛                  (1.2)
 *                                               ▼  ┃
 *                  (2.2)  ┏━━━━━<host>━━━━◀ CColorButton ◀━━━━━━━━━━━━━━━━━━━<color>
 *                         ┃                      ▲
 *                         ┃  ┏━━━━<target>━━━━━━━┛                                   (1.1)
 *                         ▼  ┃
 *      ┏━━━<host>━━━◀  CColorPicker ◀━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━<color> (1)
 *      ▼
 *   nullptr
 */


class IColorTarget
{
protected:
    IColorTarget* m_pColorTarget;
    IColorTarget*   m_pColorHost;
    bool             m_bSyncLoop;

    WCDAFX_API virtual ~IColorTarget();
    WCDAFX_API IColorTarget();

    WCDAFX_API static void SyncTargets(IColorTarget* pCurrent, COLORREF crColor, int nAlpha, bool& bInLoop);
    WCDAFX_API static void SyncHosts(IColorTarget* pCurrent, COLORREF crColor, int nAlpha, bool& bInLoop);
    WCDAFX_API virtual void SyncTargets(IColorTarget* rSource);
    WCDAFX_API virtual void SyncHosts(IColorTarget* rSource);

public:
    WCDAFX_API virtual COLORREF GetColorRef() const = 0;
    WCDAFX_API virtual int GetAlpha() const = 0;
    WCDAFX_API virtual void SetColor(COLORREF crColor, int nAlpha) = 0;

    WCDAFX_API virtual void SetColorTarget(IColorTarget& rTarget);

    DELETE_COPY_MOVE_OF(IColorTarget);
};
#endif
