#include "stdafx.h"
#include "CStaticRes.h"
#include "static-rez/IconPicker.h"
#include <dh.tracing.h>
#include <dh.tracing.defs.h>
#include <string.utils.error.code.h>
#include <atlimage.h>
#include <wil/com.h>
#include <memory>
#include <string>

CStaticRes const& CStaticRes::Instance()
{
    static CStaticRes const gs_Inst;
    return gs_Inst;
}

CStaticRes::~CStaticRes() = default;

CStaticRes::CStaticRes()
{
    UNREFERENCED_PARAMETER(ATL::ATLImplementationDetails::InitializeCImage);
    MakeBitmaps();
}

template <int CX, int CY, int BPP>
struct TBitmapDef
{
    BYTE bData[CX*CY*BPP];

    constexpr void Create(WTL::CBitmap& bmTarget) const
    {
        bmTarget.CreateBitmap(CX, CY, 1, BPP, bData);
    }
};

static bool StaticLoadIcon(BYTE const* pBytes, size_t nSize, WTL::CIcon& icoTarget)
{
    using  HGlobal = std::shared_ptr<void>;
    using PIStream = wil::com_ptr_nothrow<IStream>;
    PIStream   pStream{};
    LPVOID      pImage{nullptr};
    HRESULT      hCode{S_OK};
    std::wstring sFunc{L"NONE"};
    HGlobal const hMem{GlobalAlloc(GMEM_MOVEABLE, nSize), GlobalFree};
    if (!hMem) {
        hCode = static_cast<HRESULT>(GetLastError());
        sFunc = L"GlobalAlloc";
        goto reportError;
    }
    pImage = GlobalLock(hMem.get());
    if (!pImage) {
        hCode = static_cast<HRESULT>(GetLastError());
        sFunc = L"GlobalLock";
        goto reportError;
    }
    memcpy(pImage, pBytes, nSize);
    GlobalUnlock(hMem.get());
    hCode = CreateStreamOnHGlobal(hMem.get(), TRUE, &pStream);
    if (FAILED(hCode)) {
        sFunc = L"CreateStreamOnHGlobal";
        goto reportError;
    }
    {
        HICON             hIco{nullptr};
        Gdiplus::Bitmap bitmap{pStream.get()};
        auto const  gdipStatus{bitmap.GetHICON(&hIco)};
        if (Gdiplus::Status::Ok != gdipStatus) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc = L"Gdiplus::Bitmap::GetHICON(HICON*)";
            goto reportError;
        }
        icoTarget.Attach(hIco);
        return true;
    }
reportError:
    auto const  message{Str::ErrorCode<>::SystemMessage(hCode)};
    DH::TPrintf(LTH_GLOBALS L" %s failed: 0x%08x %s\n", sFunc.c_str(), hCode, message.GetString());
    return false;
}

void CStaticRes::MakeBitmaps()
{
    StaticLoadIcon(gs_baPickerIcon, gs_baPickerIconSize, m_icoPicker);

    constexpr TBitmapDef<11, 11, 1> bCross{0xBF, 0xA0, 0x1F, 0x00, 0x8E, 0x20,
                                           0xC4, 0x60, 0xE0, 0xE0, 0xF1, 0xE0,
                                           0xE0, 0xE0, 0xC4, 0x60, 0x8E, 0x20,
                                           0x1F, 0x00, 0xBF, 0xA0};
    bCross.Create(m_bmCross);
}
