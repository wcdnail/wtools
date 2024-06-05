#include "stdafx.h"
#include "luicSchemeManager.h"
#include "luicIniParser.h"
#include "luicUtils.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>
#include <string.utils.format.h>
#include <functional>
#include <fstream>
#include <codecvt>

namespace
{
    static constexpr wchar_t NewSchemeBegin[] = L"HKEY_CURRENT_USER\\Control Panel\\Appearance\\New Schemes";

    enum SectionParam
    {
        SchemeIndex = 0,
        SchemeSizes,
        SchemeSizeIndex,
        SectionParamCount
    };

    using UTF16LECodeCvt = std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>;
    using    WBufferConv = std::wbuffer_convert<UTF16LECodeCvt>;
}

static IniStringW WinXPRegValueStr(IniStringW const& sValue, DWORD& dwType)
{
    const size_t end{sValue.length()};
    auto         beg{sValue.find(L":")};
    if (IniStringW::npos == beg) {
        beg = 0;
        dwType = REG_SZ;
    }
    else {
        auto const& sType{sValue.substr(0, beg)};
        if (0 == _wcsnicmp(sType.c_str(), L"dword", _countof(L"dword"))) {
            dwType = REG_DWORD;
        }
        else if (0 == _wcsnicmp(sType.c_str(), L"hex(b)", _countof(L"hex(b)"))) {
            dwType = REG_QWORD;
        }
        else if (0 == _wcsnicmp(sType.c_str(), L"hex", _countof(L"hex"))) {
            dwType = REG_BINARY;
        }
        ++beg;
        if (beg >= end) {
            return {};
        }
    }
    return sValue.substr(beg, end - beg);
}

static size_t WinXPRegStrToBinary(IniStringW const& str, LPBYTE data, size_t size)
{
    if (str.empty()) {
        return 0;
    }
    size_t     count{0};
    size_t     begin{0};
    const size_t len{str.length()};
    while (count < size && begin < len) {
        auto end = str.find(L',', begin);
        if (IniString::npos == end) {
            end = len;
        }
        const BYTE value{static_cast<BYTE>(std::stoi(str.substr(begin, end - begin), nullptr, 16))};
        data[count] = value;
        ++count;
        begin = end + 1;
    }
    return count;
}

static bool WinXPRegDWORD(IniStringW const& sValue, DWORD& dwTarget)
{
    DWORD      dwType{REG_NONE};
    auto const& sTemp{WinXPRegValueStr(sValue, dwType)};
    if (REG_DWORD != dwType) {
        dwTarget = static_cast<DWORD>(-1);
        return false;
    }
    dwTarget = std::stoi(sTemp, nullptr, 16);
    return true;
}

static bool WinXPRegQWORD(IniStringW const& sValue, UINT64& dwTarget)
{
    DWORD      dwType{REG_NONE};
    auto const& sTemp{WinXPRegValueStr(sValue, dwType)};
    if (REG_QWORD != dwType) {
        dwTarget = static_cast<UINT64>(-1);
        return false;
    }
    return sizeof(dwTarget) == WinXPRegStrToBinary(sTemp, reinterpret_cast<LPBYTE>(&dwTarget), sizeof(dwTarget));
}

static bool WinXPRegBinary(IniStringW const& sValue, LPBYTE data, size_t size)
{
    DWORD      dwType{REG_NONE};
    auto const& sTemp{WinXPRegValueStr(sValue, dwType)};
    if (REG_BINARY != dwType) {
        memset(data, -1, size);
        return false;
    }
    return size == WinXPRegStrToBinary(sTemp, data, size);
}

static bool WinXPRegLoadColors(CSchemePtr const& pScheme, IniMapW& iniMap)
{
    CColorPair clrPair[CLR_Count];
    for (int i = 0; i < CLR_Count; i++) {
        IniStringW name = L"\"Color #" + std::to_wstring(i) + L"\"";
        DWORD dwColor = CLR_INVALID;
        if (!WinXPRegDWORD(StrUnquote(iniMap[name]), dwColor)) {
            dwColor = GetSysColor(i); // TODO: get WINXP pallete 
        }
        if (!clrPair[i].Reset(static_cast<COLORREF>(dwColor))) {
            SetLastError(ERROR_BADKEY);
            return false;
        }
    }
    for (int i = 0; i < CLR_Count; i++) {
        pScheme->GetColorPair(i).Swap(clrPair[i]);
    }
    return true;
}

static bool WinXPRegLoadNCMectrics(CNCMetrics& ncMetrics, CFonts& fonts, IniMapW& iniMap)
{
    for (int i = 0; i < NCM_PaddedBorder; i++) {
        IniStringW name{L"\"Size #" + std::to_wstring(i) + L"\""};
        UINT64 qwMetric{0};
        if (!WinXPRegQWORD(StrUnquote(iniMap[name]), qwMetric)) {
            SetLastError(ERROR_CANTOPEN);
            return false;
        }
        if (g_DPI() != USER_DEFAULT_SCREEN_DPI) {
            ncMetrics[i] = ScaleForDpi<int>(static_cast<int>(qwMetric));
        }
        else {
            ncMetrics[i] = static_cast<int>(qwMetric);
        }
    }
    WTL::CLogFont logFontDesktop;
    LOGFONTW* logFont[FONT_Hyperlink] = {
        &ncMetrics.lfCaptionFont,
        &ncMetrics.lfSmCaptionFont,
        &ncMetrics.lfMenuFont,
        &ncMetrics.lfStatusFont,
        &ncMetrics.lfMessageFont,
        &logFontDesktop,
    };
    for (int i = 0; i < FONT_Hyperlink; i++) {
        IniStringW name{L"\"Font #" + std::to_wstring(i) + L"\""};
        if (!WinXPRegBinary(StrUnquote(iniMap[name]), reinterpret_cast<LPBYTE>(logFont[i]), sizeof(LOGFONTW))) {
            SetLastError(ERROR_CANTOPEN);
            return false;
        }
    }
    if (!fonts.LoadValues(logFontDesktop)) {
        SetLastError(ERROR_CANTOPEN);
        return false;
    }
    return fonts.LoadValues(ncMetrics);
}

static bool WinXPRegParseScheme(CSchemeManager const& manager, CSchemePtr& pScheme, IniStringW& sCurrentName, IniMapW& iniMap)
{
    CNCMetrics ncMetrics{};
    CFonts      schFonts{};
    DWORD    dwFlatMenus{0};
    auto&    sLegacyName{iniMap[L"\"LegacyName\""]};
    if (sLegacyName.empty()) {
        sLegacyName = sCurrentName;
    }
    if (sLegacyName.empty()) {
        SetLastError(ERROR_BADDB);
        return false;
    }
    const int nCount{manager.CountWithSameName(StrUnquote(sLegacyName))};
    pScheme = std::make_shared<CScheme>(sLegacyName, nCount);
    sCurrentName = pScheme->Name();
    pScheme->SetGradientCaptions(true);
    if (WinXPRegDWORD(StrUnquote(iniMap[L"\"Flat Menus\""]), dwFlatMenus)) {
        pScheme->SetFlatMenus(0 != dwFlatMenus);
    }
    if (!WinXPRegLoadColors(pScheme, iniMap)) {
        return false;
    }
    if (!WinXPRegLoadNCMectrics(ncMetrics, schFonts, iniMap)) {
        return false;
    }
    CSizePair& sizePair = pScheme->GetSizesMap()[L"Default"];
    ncMetrics.CopyTo(sizePair.m_NCMetric);
    schFonts.Swap(sizePair.m_Font);
    return true;
}

int CSchemeManager::LoadXPRegistry(Path const& path)
{
    HRESULT         code{S_OK};
    const auto& pathname{path.native()};
    InutFStream  fsInput{pathname, std::ios::in | std::ios::binary};
    if (!fsInput.is_open()) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"ERROR: open winxp registry file FAILED!\r\n'%s'\r\n",
            pathname.c_str()), code, true);
        return IT_Invalid;
    }
    WBufferConv   fsBufConv{fsInput.rdbuf()};
    std::wistream  fsInputW{&fsBufConv};
    IniParserW    iniParser{};
    IniStringW sCurrentName{};
    SchemeVec   tempSchemes{};
    tempSchemes.reserve(m_Schemes.size() + 1);
    iniParser.parse(fsInputW);
    for (auto& sd: iniParser.sections) {
        auto const& sectName{sd.first};
        if (0 == _wcsnicmp(sectName.c_str(), NewSchemeBegin, _countof(NewSchemeBegin)-1)) {
            IniStringW sParam[SectionParamCount];
            int        iParam{SchemeIndex};
            size_t       nBeg{_countof(NewSchemeBegin)};
            size_t       nEnd{0};
            for (; (iParam < SectionParamCount) && (nBeg < sectName.length()); iParam++) {
                nEnd = sectName.find(L'\\', nBeg);
                if (IniStringW::npos == nEnd) {
                    nEnd = sectName.length();
                }
                sParam[iParam] = sectName.substr(nBeg, nEnd - nBeg);
                nBeg = nEnd + 1;
            }
            if (SchemeSizes == iParam) {
                sCurrentName = sd.second[L"\"DisplayName\""];
                continue;
            }
            if (iParam < SchemeSizeIndex) {
                continue;
            }
            if (sCurrentName.empty()) {
                sCurrentName = sParam[SchemeIndex];
            }
            CSchemePtr pScheme{};
            if (!WinXPRegParseScheme(*this, pScheme, sCurrentName, sd.second)) {
                code = static_cast<HRESULT>(GetLastError());
                ReportError(Str::ElipsisW::Format(L"Scheme Load failed\r\n'%s'\r\n",
                    sCurrentName.c_str()), code, true, MB_ICONERROR);
                return IT_Invalid;
            }
            tempSchemes.emplace_back(std::move(pScheme));
            IniStringW{}.swap(sCurrentName);
        }
    }
    tempSchemes.append_range(m_Schemes);
    tempSchemes.swap(m_Schemes);
    return 0;
}
