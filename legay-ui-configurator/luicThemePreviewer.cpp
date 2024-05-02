#include "stdafx.h"
#include "luicThemePreviewer.h"
#include "luicMain.h"
#include "string.utils.format.h"
#include "dh.tracing.h"

class DrawRoutines
{
    HMODULE USER32;

public:
    using DrawCaptionTempWFn = BOOL(WINAPI*)(HWND hWnd, HDC hdc, const RECT* pRect, HFONT hFont, HICON hIcon, WCHAR* str, UINT uFlags);
    using SetSysColorsTempFn = DWORD_PTR(WINAPI*)(const COLORREF* pPens, const HBRUSH* pBrushes, DWORD_PTR n);
    using  DrawMenuBarTempFn = int(WINAPI*)(HWND hWnd, HDC hdc, RECT* pRect, HMENU hMenu, HFONT hFont);

    DrawCaptionTempWFn DrawCaptionTempW;
    SetSysColorsTempFn SetSysColorsTemp;
    DrawMenuBarTempFn   DrawMenuBarTemp;

    static DrawRoutines& instance();

private:
    ~DrawRoutines();
    DrawRoutines();

    void Init();
};

DrawRoutines& DrawRoutines::instance()
{
    static DrawRoutines inst;
    return inst;
}

DrawRoutines::~DrawRoutines()
{
    if (USER32) {
        FreeLibrary(USER32);
    }
}

DrawRoutines::DrawRoutines()
    :           USER32{ LoadLibraryW(L"USER32") }
    , DrawCaptionTempW{ nullptr }
    , SetSysColorsTemp{ nullptr }
    ,  DrawMenuBarTemp{ nullptr }

{
    Init();
}

template <typename T>
static inline bool GetProcAddressEx(HMODULE hMod, T& routine, PCSTR routineName, PCSTR modAlias)
{
    *reinterpret_cast<FARPROC*>(&routine) = GetProcAddress(hMod, routineName);
    if (!routine) {
        DH::ThreadPrintf("%14s| '%s' is NULL\n", modAlias, routineName);
    }
    return nullptr != routine;
}

#define GetProcAddressEX(Module, Routine) \
    GetProcAddressEx(Module, Routine, #Routine, #Module)

void DrawRoutines::Init()
{
    GetProcAddressEX(USER32, DrawCaptionTempW);
    GetProcAddressEX(USER32, SetSysColorsTemp);
    GetProcAddressEX(USER32, DrawMenuBarTemp);
}

CThemePreviewer::~CThemePreviewer()
{
}

CThemePreviewer::CThemePreviewer()
{
    auto& classInfo = GetWndClassInfo();
    ATOM atom = classInfo.Register(&this->m_pfnSuperWindowProc);
    if (!atom) {
        HRESULT code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Registering '%s' failure!", classInfo.m_wc.lpszClassName), code, true, MB_ICONERROR);
    }

    UNREFERENCED_PARAMETER(DrawRoutines::instance());
}

void CThemePreviewer::OnPaint(CDCHandle dcParam)
{
    UNREFERENCED_PARAMETER(dcParam);

    const auto& theme = CLegacyUIConfigurator::App()->CurrentTheme();

    CPaintDC dc(m_hWnd);
    // ##TODO: later via CreateCompatibleDC

    CRect rcClient;
    GetClientRect(rcClient);

    dc.FillSolidRect(rcClient, theme.m_Color[COLOR_BACKGROUND]);
}
