#pragma once

#include "CColorPickerDefs.h"
#include "CDDCtl.h"
#include <wcdafx.api.h>
#include <atlgdi.h>
#include <utility>

struct CRGBSpecRect;

struct CSpectrumImage: CDDCtrl
{
    DELETE_COPY_MOVE_OF(CSpectrumImage);

    DECLARE_WND_SUPERCLASS2(CSPECIMG_CLASS, CSpectrumImage, nullptr)

    WCDAFX_API ~CSpectrumImage() override;
    WCDAFX_API CSpectrumImage(COLORREF crInit, SpectrumKind kind);

    WCDAFX_API HRESULT PreCreateWindow() override;
    WCDAFX_API bool Initialize(long cx, long cy, HBRUSH brBackground);
    WCDAFX_API COLORREF GetColorRef() const;
    WCDAFX_API COLORREF GetMinColorRef(int mR, int mG, int mB) const;
    WCDAFX_API CColorUnion& GetColor();
    WCDAFX_API void SetSpectrumKind(SpectrumKind kind);
    WCDAFX_API SpectrumKind GetSpectrumKind() const;
    WCDAFX_API SpectrumKind const& GetSpectrumKindRef() const;
    WCDAFX_API void OnColorChanged(double xPos, double yPos);

private:
    CColorUnion  m_Color;
    SpectrumKind m_nKind;

    void UpdateRaster();
    void DrawMarker(WTL::CDCHandle dc, CRect const& rc);
    void DoPaint(WTL::CDCHandle dc, CRect const& rc) override;
    void DoNotifyMouseOver(CRect const& rc, CPoint pt) override;
    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) override;
};

inline SpectrumKind CSpectrumImage::GetSpectrumKind() const
{
    return m_nKind;
}

inline SpectrumKind const& CSpectrumImage::GetSpectrumKindRef() const
{
    return m_nKind;
}

inline CColorUnion& CSpectrumImage::GetColor()
{
    return m_Color;
}

inline COLORREF CSpectrumImage::GetColorRef() const
{
    return m_Color.GetColorRef();
}

inline COLORREF CSpectrumImage::GetMinColorRef(int mR, int mG, int mB) const
{
    return RGB(std::max<BYTE>(mR, m_Color.m_R),
               std::max<BYTE>(mG, m_Color.m_G),
               std::max<BYTE>(mB, m_Color.m_B));
}
