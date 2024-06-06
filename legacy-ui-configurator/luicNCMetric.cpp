#include "stdafx.h"
#include "luicNCMetric.h"
#include "luicFonts.h"
#include <win32.registry.h>
#include "dh.tracing.h"

CNCMetrics::~CNCMetrics() = default;

CNCMetrics::CNCMetrics()
{
    static_assert(sizeof(*this) == sizeof(NONCLIENTMETRICS), "NONCLIENTMETRICS SIZEOF did not MATCH!");
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
        DH::TPrintf(L"ERROR", L"%s: index [%d] out of range\n", __FUNCTIONW__, index);
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
        DH::TPrintf(L"ERROR", L"%s: index [%d] out of range\n", __FUNCTIONW__, index);
        return L"INVALID METRIC";
    }
    return gs_name[index];
}

bool CNCMetrics::LoadDefaults()
{
    CNCMetrics ncMetrics;
    const BOOL ret{SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncMetrics), &ncMetrics, 0)};
    if (!ret) {
        return false;
    }
    return LoadValues(ncMetrics);
}

bool CNCMetrics::LoadValues(CNCMetrics const& ncMetrics)
{
    CopyMemory(this, &ncMetrics, sizeof(ncMetrics));
    return true;
}

static void ConvertLogFont(LOGFONTW& target, LOGFONTA const& source)
{
    target.lfHeight = source.lfHeight;
    target.lfWidth = source.lfWidth;
    target.lfEscapement = source.lfEscapement;
    target.lfOrientation = source.lfOrientation;
    target.lfWeight = source.lfWeight;
    target.lfItalic = source.lfItalic;
    target.lfUnderline = source.lfUnderline;
    target.lfStrikeOut = source.lfStrikeOut;
    target.lfCharSet = source.lfCharSet;
    target.lfOutPrecision = source.lfOutPrecision;
    target.lfClipPrecision = source.lfClipPrecision;
    target.lfQuality = source.lfQuality;
    target.lfPitchAndFamily = source.lfPitchAndFamily;
    MultiByteToWideChar(CP_ACP, 0, source.lfFaceName, LF_FACESIZE, target.lfFaceName, LF_FACESIZE);
}

bool CNCMetrics::LoadValues(NONCLIENTMETRICSA const& ncMetrics)
{
    CNCMetrics temp;
    temp.iBorderWidth = ncMetrics.iBorderWidth;
    temp.iScrollWidth = ncMetrics.iScrollWidth;
    temp.iScrollHeight = ncMetrics.iScrollHeight;
    temp.iCaptionWidth = ncMetrics.iCaptionWidth;
    temp.iCaptionHeight = ncMetrics.iCaptionHeight;
    temp.iSmCaptionWidth = ncMetrics.iSmCaptionWidth;
    temp.iSmCaptionHeight = ncMetrics.iSmCaptionHeight;
    temp.iMenuWidth = ncMetrics.iMenuWidth;
    temp.iMenuHeight = ncMetrics.iMenuHeight;
    temp.iPaddedBorderWidth = ncMetrics.iPaddedBorderWidth;
    ConvertLogFont(temp.lfCaptionFont, ncMetrics.lfCaptionFont);
    ConvertLogFont(temp.lfSmCaptionFont, ncMetrics.lfSmCaptionFont);
    ConvertLogFont(temp.lfMenuFont, ncMetrics.lfMenuFont);
    ConvertLogFont(temp.lfStatusFont, ncMetrics.lfStatusFont);
    ConvertLogFont(temp.lfMessageFont, ncMetrics.lfMessageFont);
    temp.CopyTo(*this);
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

    LOGFONTW* logFont[FONT_Desktop] = {
        &this->lfCaptionFont,
        &this->lfSmCaptionFont,
        &this->lfMenuFont,
        &this->lfStatusFont,
        &this->lfMessageFont,
    };

    for (int i = 0; i < _countof(logFont); i++) {
        std::wstring name = L"Font" + std::to_wstring(i);
        LOGFONTW tempLogFont;
        if (!regScheme.GetValue<LOGFONTW>(name, tempLogFont)) {
            return false;
        }
        *logFont[i] = tempLogFont;
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
    DH::TPrintf(L"ERROR", L"%s: index [%d] out of range\n", __FUNCTIONW__, index);
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

bool CNCMetrics::IsNotEqual(CNCMetrics const& rhs) const
{
    return sizeof(*this) != RtlCompareMemory(this, &rhs, sizeof(*this));
}

