//
//  Class:      CDibEx
//
//  Compiler:   Visual C++
//              eMbedded Visual C++
//  Tested on:  Visual C++ 6.0
//              Windows CE 3.0
//
//  Author:     Davide Calabro'     davide_calabro@yahoo.com
//                                  http://www.softechsoftware.it
//
//  Note:       This class uses code snippets taken from a similar class written
//              for the Win32 enviroment by Davide Pizzolato (ing.davide.pizzolato@libero.it)
//
//  Disclaimer
//  ----------
//  THIS SOFTWARE AND THE ACCOMPANYING FILES ARE DISTRIBUTED "AS IS" AND WITHOUT
//  ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED. NO REPONSIBILITIES FOR POSSIBLE
//  DAMAGES OR EVEN FUNCTIONALITY CAN BE TAKEN. THE USER MUST ASSUME THE ENTIRE
//  RISK OF USING THIS SOFTWARE.
//
//  Terms of use
//  ------------
//  THIS SOFTWARE IS FREE FOR PERSONAL USE OR FREEWARE APPLICATIONS.
//  IF YOU USE THIS SOFTWARE IN COMMERCIAL OR SHAREWARE APPLICATIONS YOU
//  ARE GENTLY ASKED TO DONATE 1$ (ONE U.S. DOLLAR) TO THE AUTHOR:
//
//      Davide Calabro'
//      P.O. Box 65
//      21019 Somma Lombardo (VA)
//      Italy
//
//

#pragma once

#include <wcdafx.api.h>
#include <cstdint>
#include <memory>

template <typename T>
static constexpr auto BytesInBPP(T bits) -> T
{
    return (((bits) + 31) / 32 * 4);
}

static constexpr RGBQUAD RGB2RGBQUAD(COLORREF cr)
{
    RGBQUAD c;
    c.rgbRed = GetRValue(cr);   /* get R, G, and B out of DWORD */
    c.rgbGreen = GetGValue(cr);
    c.rgbBlue = GetBValue(cr);
    c.rgbReserved=0;
    return c;
} // End of RGB2RGBQUAD


struct CDibEx
{
    using LPDIB = uint8_t*;

    DELETE_COPY_MOVE_OF(CDibEx);

    virtual ~CDibEx();
    CDibEx();

    LPBYTE GetData() const;
    LONG GetWidth() const;
    LONG GetHeight() const;
    DWORD GetStride() const;
    WORD GetBitCount() const;
    LONG GetNumColors() const;

    LPDIB Create(LONG nWidth, LONG nHeight, LONG nBitCount);
    void Clear(int byVal = 0) const;
    void Swap(CDibEx& rhs) noexcept;
    bool Clone(CDibEx const& src);

    void SetGrayPalette() const;
    void SetPaletteIndex(DWORD byIdx, BYTE byR, BYTE byG, BYTE byB) const;
    void SetPixelIndex(LONG dwX, LONG dwY, BYTE byI) const;
    void BlendPalette(COLORREF crColor, DWORD dwPerc) const;

    BOOL WriteBMP(LPCTSTR bmpFileName) const;

protected:
    using DIBPtr = std::unique_ptr<uint8_t[]>;

    LPBITMAPINFOHEADER GetInfoHdr() const;
    LPBYTE GetPaletteBits() const;

    static DWORD GetPaletteSize(LONG wColors);
    static DWORD GetSize(BITMAPINFOHEADER const& biHdr, LONG wColors);

    DIBPtr  m_pDib;
    LONG m_nStride;
    LONG m_nColors;
};

struct CDIBitmap: CDibEx
{
    DELETE_COPY_MOVE_OF(CDIBitmap);

    ~CDIBitmap() override;
    CDIBitmap();

    void Swap(CDIBitmap& rhs) noexcept;

    WTL::CDCHandle GetDC(WTL::CDCHandle dc);
    void Draw(WTL::CDCHandle dc, CRect const& rc);
    void Borrow(WTL::CDCHandle dc, int dwX, int dwY);

private:
    WTL::CDC         m_DC;
    WTL::CBitmap m_Bitmap;
    HBITMAP      m_hOldBm;
};
