#include "stdafx.h"
#include "CeXDib.h"

CDibEx::~CDibEx() = default;

CDIBitmap::~CDIBitmap()
{
    if (m_DC && m_hOldBm) {
        m_DC.SelectBitmap(m_hOldBm);
    }
}

CDibEx::CDibEx()
    :    m_pDib{nullptr}
    , m_nStride{0}
    , m_nColors{0}
{
}

CDIBitmap::CDIBitmap()
    :     m_DC{}
    , m_Bitmap{}
    , m_hOldBm{nullptr}
{
}

DWORD CDibEx::GetSize(BITMAPINFOHEADER const& biHdr, LONG wColors)
{
    return biHdr.biSize + biHdr.biSizeImage + GetPaletteSize(wColors);
} // End of GetSize

DWORD CDibEx::GetImageSize() const
{
    if (!m_pDib) {
        return 0;
    }
    return GetSize(*GetInfoHdr(), m_nColors);
}

DWORD CDibEx::GetPaletteSize(LONG wColors)
{
    return (wColors * sizeof(RGBQUAD));
} // End of GetPaletteSize

LPBITMAPINFOHEADER CDibEx::GetInfoHdr() const
{
    return reinterpret_cast<LPBITMAPINFOHEADER>(m_pDib.get());
} // End of GetInfoHdr

LPBYTE CDibEx::GetPaletteBits() const
{
    if (!m_pDib) {
        return nullptr;
    }
    return (m_pDib.get() + GetInfoHdr()->biSize);
} // End of GetData

LPBYTE CDibEx::GetData() const
{
    if (!m_pDib) {
        return nullptr;
    }
    return (GetPaletteBits() + GetPaletteSize(m_nColors));
} // End of GetData

LONG CDibEx::GetWidth() const
{
    return m_pDib ? GetInfoHdr()->biWidth : 0;
} // End of GetWidth

LONG CDibEx::GetHeight() const
{
    return m_pDib ? GetInfoHdr()->biHeight : 0;
} // End of GetHeight

DWORD CDibEx::GetStride() const
{
    return m_nStride;
} // End of GetStride

WORD CDibEx::GetBitCount() const
{
    return GetInfoHdr()->biBitCount;
} // End of GetBitCount

LONG CDibEx::GetNumColors() const
{
    return m_nColors;
} // End of GetNumColors

CDibEx::LPDIB CDibEx::Create(LONG nWidth, LONG nHeight, LONG nBitCount)
{
    BITMAPINFOHEADER biHdr{0};
    DWORD       dwMemBytes{0};
    LONG           wColors{0};
    LONG const     nStride{nWidth * BytesInBPP<LONG>(nBitCount)};

    // Following <switch> is taken from
    // CDIBSectionLite class by Chris Maunder
    switch (nBitCount) {
        case 1:  wColors = 2;   break;
#ifdef _WIN32_WCE
        case 2:  wColors = 4;   break;  // winCE only       
#endif
        case 4:  wColors = 16;  break;
        case 8:  wColors = 256; break;
        case 16:
        case 24:
        case 32:
        default: wColors = 0;   break;  // 16, 24 or 32 bpp have no color table
    } // switch
#if 0
    // Make sure bits per pixel is valid
    if (nBitCount <= 1)         nBitCount = 1;
    else if (nBitCount <= 4)    nBitCount = 4;
    else if (nBitCount <= 8)    nBitCount = 8;
    else                        nBitCount = 24;

    switch (nBitCount)
    {
        case 1:
            m_nColors = 2;
            break;
        case 4:
            m_nColors = 16;
            break;
        case 8:
            m_nColors = 256;
            break;
        default:
            m_nColors = 0;
            break;
    } // switch
#endif
    // Initialize BITMAPINFOHEADER
    biHdr.biSize = sizeof(biHdr);
    biHdr.biWidth = nWidth;        // fill in width from parameter
    biHdr.biHeight = nHeight;      // fill in height from parameter
    biHdr.biPlanes = 1;            // must be 1
    biHdr.biBitCount = static_cast<WORD>(nBitCount);
    biHdr.biCompression = BI_RGB;
    biHdr.biSizeImage = nStride * nHeight;
    biHdr.biXPelsPerMeter = 0;
    biHdr.biYPelsPerMeter = 0;
    biHdr.biClrUsed = 0;
    biHdr.biClrImportant = 0;

    // Calculate size of memory block required to store the DIB.  This
    // block should be big enough to hold the BITMAPINFOHEADER, the color
    // table, and the bits.
    dwMemBytes = GetSize(biHdr, wColors);

    DIBPtr pDib = std::make_unique<uint8_t[]>(dwMemBytes);

    // Use our bitmap info structure to fill in first part of
    // our DIB with the BITMAPINFOHEADER
    *reinterpret_cast<LPBITMAPINFOHEADER>(pDib.get()) = biHdr;

    pDib.swap(m_pDib);
    m_nStride = nStride;
    m_nColors = wColors;
    return m_pDib.get(); // Return handle to the DIB
} // End of Create

void CDibEx::Clear(int byVal) const
{
    if (!m_pDib) {
        return ;
    }
    memset(GetData(), byVal, GetInfoHdr()->biSizeImage);
} // End of Clear

bool CDibEx::Clone(CDibEx const& src)
{
    if (!src.GetData()) {
        return false;
    }
    CDibEx temp{};
    if (!temp.Create(src.GetWidth(), src.GetHeight(), src.GetBitCount())) {
        return false;
    }
    memcpy(temp.m_pDib.get(), src.m_pDib.get(), GetSize(*temp.GetInfoHdr(), temp.m_nColors));
    temp.Swap(*this);
    return true;
} // End of Clone

void CDibEx::Swap(CDibEx& rhs) noexcept
{
    std::swap(m_pDib,    rhs.m_pDib);
    std::swap(m_nStride, rhs.m_nStride);
    std::swap(m_nColors, rhs.m_nColors);
}

void CDIBitmap::Swap(CDIBitmap& rhs) noexcept
{
    CDibEx::Swap(rhs);
    std::swap(m_DC.m_hDC,  rhs.m_DC.m_hDC);
    std::swap(m_Bitmap.m_hBitmap, rhs.m_Bitmap.m_hBitmap);
}

void CDibEx::BlendPalette(COLORREF crColor, DWORD dwPerc) const
{
    if (!m_pDib || !m_nColors) {
        return;
    }
    const LPBYTE pDst{GetPaletteBits()};
    RGBQUAD*     pPal{reinterpret_cast<RGBQUAD*>(pDst)};
    LONG const      R{GetRValue(crColor)};
    LONG const      G{GetGValue(crColor)};
    LONG const      B{GetBValue(crColor)};
    if (dwPerc > 100) {
        dwPerc = 100;
    }
    for (LONG i = 0; i < m_nColors; i++) {
        pPal[i].rgbBlue  = static_cast<BYTE>((pPal[i].rgbBlue  * (100 - dwPerc) + B * dwPerc) / 100);
        pPal[i].rgbGreen = static_cast<BYTE>((pPal[i].rgbGreen * (100 - dwPerc) + G * dwPerc) / 100);
        pPal[i].rgbRed   = static_cast<BYTE>((pPal[i].rgbRed   * (100 - dwPerc) + R * dwPerc) / 100);
    } // for
} // End of BlendPalette

void CDibEx::SetPixelIndex(LONG dwX, LONG dwY, BYTE byI) const
{
    if (!m_pDib || !m_nColors ||
        (dwX < 0) || (dwY < 0) ||
        (dwX >= GetInfoHdr()->biWidth) ||
        (dwY >= GetInfoHdr()->biHeight)) {
        return;
    }

    const LPBYTE iDst = GetData();
    iDst[(GetInfoHdr()->biHeight - dwY - 1) * m_nStride + dwX] = byI;
} // End of SetPixelIndex

void CDibEx::SetPaletteIndex(DWORD byIdx, BYTE byR, BYTE byG, BYTE byB) const
{
    if (!m_pDib || !m_nColors || (static_cast<LONG>(byIdx) > m_nColors - 1)) {
        return ;
    }
    const LPBYTE iDst{GetPaletteBits()};
    DWORD     lOffset{byIdx * static_cast<DWORD>(sizeof(RGBQUAD))};
    iDst[lOffset++] = byB;
    iDst[lOffset++] = byG;
    iDst[lOffset++] = byR;
    iDst[lOffset] = 0;
} // End of SetPaletteIndex

void CDibEx::SetGrayPalette() const
{
    if (!m_pDib || !m_nColors) {
        return;
    }
    RGBQUAD    pal[256]{0};
    RGBQUAD const* ppal{pal};
    for (int ni = 0; ni < m_nColors; ni++) {
        pal[ni] = RGB2RGBQUAD(RGB(ni,ni,ni));
    } // for
    pal[0] = RGB2RGBQUAD(RGB(0,0,0));
    pal[m_nColors-1] = RGB2RGBQUAD(RGB(255,255,255));
    const LPBYTE iDst{GetPaletteBits()};
    memcpy(iDst, ppal, GetPaletteSize(m_nColors));
} // End of SetGrayPalette

BOOL CDibEx::WriteBMP(LPCTSTR bmpFileName) const
{
    static constexpr WORD BITMAP_HDR = 0x4d42; // 'BM'

    if (*bmpFileName == _T('\0') || !m_pDib) {
        return 0;
    }
    const DWORD dwMySize{GetSize(*GetInfoHdr(), m_nColors)};
    const HANDLE   hFile{CreateFile(    // open if exist ini file
        bmpFileName,                    // pointer to name of the file 
        GENERIC_WRITE,                  // access mode 
        0,                              // share mode 
        nullptr,                        // pointer to security descriptor 
        CREATE_ALWAYS,                  // how to create 
        FILE_ATTRIBUTE_NORMAL,          // file attributes 
        nullptr)};                      // handle to file with attributes to copy  
    if (INVALID_HANDLE_VALUE == hFile) {
        return FALSE;
    }

    // Fill in the fields of the file header
    BITMAPFILEHEADER hdr{0};
    hdr.bfType = BITMAP_HDR;
    hdr.bfSize = dwMySize + sizeof(BITMAPFILEHEADER);
    hdr.bfReserved1 = hdr.bfReserved2 = 0;
    hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + GetInfoHdr()->biSize + GetPaletteSize(m_nColors);

    // Write the file header
    DWORD nByteWrite{0};
    WriteFile(                      // write ini (sync mode <-> no overlapped)
        hFile,                      // handle of file to write 
        (LPCVOID)&hdr,              // address of buffer that contains data  
        sizeof(BITMAPFILEHEADER),   // number of bytes to write 
        &nByteWrite,                // address of number of bytes written 
        nullptr);                   // address of structure for data 

    // Write the DIB header and the bits
    WriteFile(                      // write ini (sync mode <-> no overlapped)
        hFile,                      // handle of file to write 
        (LPCVOID)m_pDib.get(),      // address of buffer that contains data  
        dwMySize,                   // number of bytes to write 
        &nByteWrite,                // address of number of bytes written 
        nullptr);                   // address of structure for data 

    CloseHandle(hFile);             // release file handle
    return TRUE;
} // End of WriteBMP

WTL::CDCHandle CDIBitmap::GetDC(WTL::CDCHandle dc)
{
    if (!m_Bitmap) {
        LPVOID    lpBits{nullptr};
        WTL::CBitmap bmp{};
        bmp.CreateDIBSection(dc, reinterpret_cast<BITMAPINFO*>(m_pDib.get()), DIB_RGB_COLORS, &lpBits, nullptr, 0);
        if (!bmp.m_hBitmap || !lpBits) {
            return {};
        }
        m_Bitmap.Attach(bmp.Detach());
        memcpy(lpBits, GetData(), GetInfoHdr()->biSizeImage);
    }
    if (!m_DC) {
        WTL::CDC dcTemp{CreateCompatibleDC(dc)};
        if (!dcTemp) {
            return {};
        }
        m_DC.Attach(dcTemp.Detach());
        m_hOldBm = m_DC.SelectBitmap(m_Bitmap);
    }
    return m_DC.m_hDC;
}

void CDIBitmap::Draw(WTL::CDCHandle dc, CRect const& rc)
{
    WTL::CDCHandle dcSource{GetDC(dc)};
    if (!dcSource) {
        return;
    }
    dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), dcSource, 0, 0, SRCCOPY);
} // End of Draw

void CDIBitmap::Borrow(WTL::CDCHandle dc, int dwX, int dwY)
{
    const WTL::CDCHandle dcSource{GetDC(dc)};
    if (!dcSource) {
        return;
    }
    WTL::CDC memDC{CreateCompatibleDC(dc)};
    auto const hBm{memDC.SelectBitmap(m_Bitmap)};
    memDC.BitBlt(0, 0, GetInfoHdr()->biWidth, GetInfoHdr()->biHeight, dc, dwX, dwY, SRCCOPY);
    memDC.SelectBitmap(hBm);
} // End of Borrow

void CDIBitmap::FreeResources()
{
    m_Bitmap.Attach(nullptr);
    if (m_hOldBm) {
        m_DC.SelectBitmap(m_hOldBm);
    }
    m_DC.Attach(nullptr);
}
