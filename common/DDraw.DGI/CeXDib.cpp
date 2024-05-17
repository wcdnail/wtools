#include "stdafx.h"
#include "CeXDib.h"

CCeXDib::CCeXDib()
    :        m_pDib{nullptr}
    , m_nStride{0}
    ,     m_nColors{0}
    ,      m_hMemDC{nullptr}
    ,     m_hBitmap{nullptr}
    ,      m_lpBits{nullptr}
{
}

CCeXDib::~CCeXDib()
{
    FreeResources();
}

void CCeXDib::FreeResources()
{
    if (m_hMemDC) {
        DeleteDC(m_hMemDC);
    }
    if (m_hBitmap) {
        DeleteObject(m_hBitmap);
    }
    DIBPtr{}.swap(m_pDib);
    m_hBitmap = nullptr;
    m_hMemDC = nullptr;
    m_lpBits = nullptr;
}

DWORD CCeXDib::GetSize(BITMAPINFOHEADER const& biHdr, LONG wColors)
{
    return biHdr.biSize + biHdr.biSizeImage + GetPaletteSize(wColors);
} // End of GetSize

DWORD CCeXDib::GetPaletteSize(LONG wColors)
{
    return (wColors * sizeof(RGBQUAD));
} // End of GetPaletteSize

LPBITMAPINFOHEADER CCeXDib::GetInfoHdr() const
{
    return reinterpret_cast<LPBITMAPINFOHEADER>(m_pDib.get());
} // End of GetInfoHdr

LPBYTE CCeXDib::GetPaletteBits() const
{
    if (!m_pDib) {
        return nullptr;
    }
    return (m_pDib.get() + GetInfoHdr()->biSize);
} // End of GetBits

LPBYTE CCeXDib::GetBits() const
{
    if (!m_pDib) {
        return nullptr;
    }
    return (GetPaletteBits() + GetPaletteSize(m_nColors));
} // End of GetBits

LONG CCeXDib::GetWidth() const
{
    return m_pDib ? GetInfoHdr()->biWidth : 0;
} // End of GetWidth

LONG CCeXDib::GetHeight() const
{
    return m_pDib ? GetInfoHdr()->biHeight : 0;
} // End of GetHeight

DWORD CCeXDib::GetStride() const
{
    return m_nStride;
} // End of GetStride

WORD CCeXDib::GetBitCount() const
{
    return GetInfoHdr()->biBitCount;
} // End of GetBitCount

LONG CCeXDib::GetNumColors() const
{
    return m_nColors;
} // End of GetNumColors

CCeXDib::LPDIB CCeXDib::Create(LONG nWidth, LONG nHeight, LONG nBitCount)
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

void CCeXDib::Swap(CCeXDib& rhs) noexcept
{
    std::swap(m_pDib,    rhs.m_pDib);
    std::swap(m_nStride, rhs.m_nStride);
    std::swap(m_nColors, rhs.m_nColors);
    std::swap(m_hMemDC,  rhs.m_hMemDC);
    std::swap(m_hBitmap, rhs.m_hBitmap);
    std::swap(m_lpBits,  rhs.m_lpBits);
}

bool CCeXDib::Clone(CCeXDib const& src)
{
    if (!src.GetBits()) {
        return false;
    }
    CCeXDib temp{};
    if (!temp.Create(src.GetWidth(), src.GetHeight(), src.GetBitCount())) {
        return false;
    }
    memcpy(temp.m_pDib.get(), src.m_pDib.get(), GetSize(*temp.GetInfoHdr(), temp.m_nColors));
    temp.Swap(*this);
    return true;
} // End of Clone

void CCeXDib::BlendPalette(COLORREF crColor, DWORD dwPerc) const
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

void CCeXDib::Clear(int byVal) const
{
    if (!m_pDib) {
        return ;
    }
    memset(GetBits(), byVal, GetInfoHdr()->biSizeImage);
} // End of Clear

void CCeXDib::SetPixelIndex(LONG dwX, LONG dwY, BYTE byI) const
{
    if (!m_pDib || !m_nColors ||
        (dwX < 0) || (dwY < 0) ||
        (dwX >= GetInfoHdr()->biWidth) ||
        (dwY >= GetInfoHdr()->biHeight)) {
        return;
    }

    const LPBYTE iDst = GetBits();
    iDst[(GetInfoHdr()->biHeight - dwY - 1) * m_nStride + dwX] = byI;
} // End of SetPixelIndex

void CCeXDib::SetPaletteIndex(DWORD byIdx, BYTE byR, BYTE byG, BYTE byB) const
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

void CCeXDib::Draw(HDC hDC, int dwX, int dwY)
{
    HBITMAP    hBitmap{nullptr};
    HBITMAP hOldBitmap{nullptr};
    HDC         hMemDC{nullptr};

    if (!m_hBitmap) {
        m_hBitmap = CreateDIBSection(hDC, reinterpret_cast<BITMAPINFO*>(m_pDib.get()), DIB_RGB_COLORS, &m_lpBits, nullptr, 0);
        if (!m_hBitmap) {
            return;
        }
        if (!m_lpBits) {
            DeleteObject(m_hBitmap);
            m_hBitmap = nullptr;
            return;
        } // if
    } // if
    if (!m_hMemDC) {
        m_hMemDC = CreateCompatibleDC(hDC);
        if (m_hMemDC == nullptr) {
            return;
        }
    } // if

    memcpy(m_lpBits, GetBits(), GetInfoHdr()->biSizeImage);
    hOldBitmap = static_cast<HBITMAP>(SelectObject(m_hMemDC, m_hBitmap));
    BitBlt(hDC, dwX, dwY, GetInfoHdr()->biWidth, GetInfoHdr()->biHeight, m_hMemDC, 0, 0, SRCCOPY);
    SelectObject(m_hMemDC, hOldBitmap);
} // End of Draw

void CCeXDib::Copy(HDC hDC, int dwX, int dwY)
{
    if (!m_hBitmap) {
        m_hBitmap = CreateDIBSection(hDC, reinterpret_cast<BITMAPINFO*>(m_pDib.get()), DIB_RGB_COLORS, &m_lpBits, nullptr, 0);
        if (!m_hBitmap) {
            return;
        }
        if (m_lpBits == nullptr) {
            DeleteObject(m_hBitmap);
            m_hBitmap = nullptr;
            return;
        } // if
    } // if
    const HDC  hMemDC{CreateCompatibleDC(hDC)};
    const HBITMAP hOB{static_cast<HBITMAP>(SelectObject(hMemDC, m_hBitmap))};
    BitBlt(hMemDC, 0, 0, GetInfoHdr()->biWidth, GetInfoHdr()->biHeight, hDC, dwX, dwY, SRCCOPY);
    SelectObject(hMemDC, hOB);
} // End of Copy

void CCeXDib::SetGrayPalette() const
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

BOOL CCeXDib::WriteBMP(LPCTSTR bmpFileName) const
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
