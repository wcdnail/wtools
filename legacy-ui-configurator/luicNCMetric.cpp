#include "stdafx.h"
#include "luicNCMetric.h"
#include "luicRegistry.h"
#include "luicFonts.h"
#include "dh.tracing.h"

CNCMetrics::~CNCMetrics() = default;

CNCMetrics::CNCMetrics()
{
    ZeroMemory(this, sizeof(*this));
    this->cbSize = sizeof(*this);
}

CNCMetrics::Range const& CNCMetrics::DefaultRange(int index)
{
    static constexpr Range gs_ranges[NCM_Count] = {
        // Index                    Min  Max  Current Orig.Max
        /* NCM_Border           */ {  1,  15,  1 },   //  50
        /* NCM_ScrollWidth      */ {  8,  70, 16 },   // 100
        /* NCM_ScrollHeight     */ {  8,  70, 16 },   // 100
        /* NCM_CaptionWidth     */ { 12,  70, 18 },   // 100
        /* NCM_CaptionHeight    */ { 12,  70, 18 },   // 100
        /* NCM_SMCaptionWidth   */ { 12,  70, 12 },   // 100
        /* NCM_SMCaptionHeight  */ { 12,  70, 15 },   // 100
        /* NCM_MenuWidth,       */ { 12,  70, 18 },   // 100
        /* NCM_MenuHeight       */ { 12,  70, 18 },   // 100
        /* NCM_PaddedBorder     */ {  0,  15,  0 }    // 100
    };
    if (index < 0 || index > _countof(gs_ranges) - 1) {
        static constexpr Range dummy{ 1, 1, 1 };
        DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
        return dummy;
    }
    return gs_ranges[index];
}

PCWSTR CNCMetrics::Title(int index)
{
    static constexpr PCWSTR gs_name[NCM_Count] = {
        L"Border Width",
        L"Scroll Width",
        L"Scroll Height",
        L"Caption Width",
        L"Caption Height",
        L"SmCaption Width",
        L"SmCaption Height",
        L"Menu Width",
        L"Menu Height",
        L"Padded BorderWidth"
    };
    if (index < 0 || index > _countof(gs_name) - 1) {
        DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
        return L"INVALID METRIC";
    }
    return gs_name[index];
}

bool CNCMetrics::LoadDefaults()
{
    NONCLIENTMETRICSW ncMetrics;
    static_assert(sizeof(*this) == sizeof(ncMetrics), "NONCLIENTMETRICS SIZEOF did not MATCH!");
    ZeroMemory(&ncMetrics, sizeof(ncMetrics));
    ncMetrics.cbSize = sizeof(NONCLIENTMETRICS);
    const BOOL ret{SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncMetrics), &ncMetrics, 0)};
    if (!ret) {
        return false;
    }
    CopyMemory(this, &ncMetrics, sizeof(ncMetrics));
    return true;
}

bool CNCMetrics::LoadValues(CRegistry const& regScheme)
{
    for (int i = 0; i < NCM_Count; i++) {
        std::wstring name = L"Size" + std::to_wstring(i);
        DWORD dwSize = static_cast<DWORD>(-1);
        if (!regScheme.GetValue<DWORD>(name, dwSize)) {
            return false;
        }
        getRefByIndex<int>(*this, i) = static_cast<int>(dwSize);
    }

    for (int i = 0; i < NCM_Count; i++) {
        std::wstring name = L"Size" + std::to_wstring(i);
        DWORD dwSize = static_cast<DWORD>(-1);
        if (!regScheme.GetValue<DWORD>(name, dwSize)) {
            return false;
        }
        getRefByIndex<int>(*this, i) = static_cast<int>(dwSize);
    }
    return true;
}

void CNCMetrics::CopyTo(CNCMetrics& target) const noexcept
{
    CopyMemory(&target, this, sizeof(*this));
}

template <typename ReturnType, typename SelfRef>
ReturnType& CNCMetrics::getRefByIndex(SelfRef& thiz, int index)
{
    switch (index) {
    case NCM_Border:           return thiz.iBorderWidth;
    case NCM_ScrollWidth:      return thiz.iScrollWidth;
    case NCM_ScrollHeight:     return thiz.iScrollHeight;
    case NCM_CaptionWidth:     return thiz.iCaptionWidth;
    case NCM_CaptionHeight:    return thiz.iCaptionHeight;
    case NCM_SMCaptionWidth:   return thiz.iSmCaptionWidth;
    case NCM_SMCaptionHeight:  return thiz.iSmCaptionHeight;
    case NCM_MenuWidth:        return thiz.iMenuWidth;
    case NCM_MenuHeight:       return thiz.iMenuHeight;
    case NCM_PaddedBorder:     return thiz.iPaddedBorderWidth;
    default:
        break;
    }
    DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
    static ReturnType dummy{NCM_Invalid};
    return dummy;
}

int& CNCMetrics::operator[](int index)
{
    return getRefByIndex<int>(*this, index);
}

int CNCMetrics::operator[](int index) const
{
    return getRefByIndex<int const>(*this, index);
}
