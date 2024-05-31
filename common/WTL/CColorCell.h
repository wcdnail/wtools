#pragma once

#include "IColor.h"
#include "IColorObserver.h"
#include <wcdafx.api.h>
#include <atlgdi.h>

class CRect;

struct CColorCell
{
    DELETE_COPY_OF(CColorCell);

    virtual ~CColorCell();
    CColorCell(COLORREF crColor, int nAlpha, HWND hHolder = nullptr);

    CColorCell(CColorCell&& rhs) noexcept;
    CColorCell& operator = (CColorCell&& rhs) noexcept;
    void Swap(CColorCell& rhs) noexcept;

    HDC GetDC(HDC dc, CRect const& rc);
    bool operator == (const CColorCell&) const;

    void Draw(WTL::CDCHandle dc, CRect const& rc, HBRUSH brBack);
    COLORREF GetColorRef() const;
    int GetAlpha() const;
    void SetColor(COLORREF crColor, int nAlpha);
    void SetHolder(HWND hHolder);

protected:
    COLORREF    m_crColor;
    int          m_nAlpha;
    HWND        m_hHolder;
    HBITMAP       m_hPrev;
    WTL::CDC         m_DC;
    WTL::CBitmap m_Bitmap;
};

struct CColorCellEx: public  IColor,
                     public  IColorObserver
{
    DELETE_COPY_MOVE_OF(CColorCellEx);

    ~CColorCellEx() override;
    CColorCellEx(COLORREF crColor, int nAlpha, HWND hHolder = nullptr);

    COLORREF GetColorRef() const override;
    int GetAlpha() const override;
    void SetColor(COLORREF crColor, int nAlpha) override;

    void Draw(WTL::CDCHandle dc, CRect const& rc, HBRUSH brBack);
    void SetHolder(HWND hHolder);

private:
    CColorCell m_clCell;

    void OnColorUpdate(IColor const& clrSource) override;
};

inline void CColorCell::SetHolder(HWND hHolder)
{
    m_hHolder = hHolder;
}

inline COLORREF CColorCell::GetColorRef() const
{
    return m_crColor;
}

inline int CColorCell::GetAlpha() const
{
    return m_nAlpha;
}

inline void CColorCellEx::Draw(WTL::CDCHandle dc, CRect const& rc, HBRUSH brBack)
{
    m_clCell.Draw(dc, rc, brBack);
}

inline void CColorCellEx::SetHolder(HWND hHolder)
{
    m_clCell.SetHolder(hHolder);
}
