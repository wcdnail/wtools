//
//  Class:      CCeXDib
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

#include <cstdint>
#include <memory>

template <typename T>
static constexpr auto BytesInBPP(T bits) -> T
{
    return (((bits) + 31) / 32 * 4);
}

struct CCeXDib
{
    using LPDIB = uint8_t*;

    virtual ~CCeXDib();
    CCeXDib();

    LPDIB Create(LONG nWidth, LONG nHeight, LONG nBitCount);
    void Swap(CCeXDib& rhs) noexcept;
    bool Clone(CCeXDib const& src);
    void Draw(HDC hDC, DWORD dwX, DWORD dwY);
    void Copy(HDC hDC, DWORD dwX, DWORD dwY);
    LPBYTE GetBits() const;
    void Clear(int byVal = 0) const;

    void SetGrayPalette();
    void SetPaletteIndex(DWORD byIdx, BYTE byR, BYTE byG, BYTE byB) const;
    void SetPixelIndex(LONG dwX, LONG dwY, BYTE byI) const;
    void BlendPalette(COLORREF crColor, DWORD dwPerc) const;

    WORD GetBitCount() const;
    DWORD GetStride() const;
    LONG GetWidth() const;
    LONG GetHeight() const;
    WORD GetNumColors();

    BOOL WriteBMP(LPCTSTR bmpFileName);

private:
    using DIBPtr = std::unique_ptr<uint8_t[]>;

    void FreeResources();
    LPBITMAPINFOHEADER GetInfoHdr() const;
    LPBYTE GetPaletteBits() const;

    static DWORD GetPaletteSize(LONG wColors);
    static DWORD GetSize(BITMAPINFOHEADER const& biHdr, LONG wColors);

    RGBQUAD RGB2RGBQUAD(COLORREF cr);

    DIBPtr         m_pDib;
    LONG        m_nStride;
    LONG        m_nColors;
    HDC          m_hMemDC;  // Handle to memory DC
    HBITMAP     m_hBitmap;  // Handle to bitmap
    LPVOID       m_lpBits;  // Pointer to actual bitmap bits
};
