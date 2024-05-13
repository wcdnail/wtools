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

    IniStringW WinXPRegValue(IniStringW const& sValue, DWORD& dwType)
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

    bool WinXPRegDWORD(IniStringW const& sValue, DWORD& dwTarget)
    {
        DWORD      dwType{REG_NONE};
        auto const& sTemp{WinXPRegValue(sValue, dwType)};
        if (REG_DWORD != dwType) {
            dwTarget = static_cast<DWORD>(-1);
            return false;
        }
        dwTarget = std::stoi(sTemp, nullptr, 16);
        return true;
    }

    bool WinXPRegParseScheme(CSchemeManager const& manager, CSchemePtr& pScheme, bool& bFound,
                             IniStringW& sCurrentName,
                             IniStringW const& sIndex,
                             IniMapW& iniMap)
    {
        auto& sLegacyName{iniMap[L"\"LegacyName\""]};
        if (sLegacyName.empty()) {
            sLegacyName = sCurrentName;
        }
        if (sLegacyName.empty()) {
            SetLastError(ERROR_BADDB);
            return false;
        }
        if (sLegacyName != sCurrentName) {
            DH::TPrintf(L"%s: Scheme legacy & current names does not match: %s != %s\n", __FUNCTIONW__,
                sLegacyName.c_str(), sCurrentName.c_str());
        }
        manager.FindOrCreate(StrUnquote(sLegacyName), pScheme);
        sCurrentName = pScheme->Name();
        pScheme->SetGradientCaptions(true);

        DWORD dwFlatMenus = 0;
        if (WinXPRegDWORD(StrUnquote(iniMap[L"\"Flat Menus\""]), dwFlatMenus)) {
            pScheme->SetFlatMenus(0 != dwFlatMenus);
        }

        CColorPair clrPair[CLR_Count];
        for (int i = 0; i < CLR_Count; i++) {
            IniStringW name = L"\"Color #" + std::to_wstring(i) + L"\"";
            DWORD dwColor = CLR_INVALID;
            if (!WinXPRegDWORD(StrUnquote(iniMap[name]), dwColor)) {
                SetLastError(ERROR_BADKEY);
                return false;
            }
            if (!clrPair[i].Reset(static_cast<COLORREF>(dwColor))) {
                SetLastError(ERROR_BADKEY);
                return false;
            }
        }
        for (int i = 0; i < CLR_Count; i++) {
            pScheme->GetColorPair(i).Swap(clrPair[i]);
        }

        CSizePair& sizePair = pScheme->GetSizesMap()[L"Default"];
        sizePair.m_Font.LoadDefaults();
        sizePair.m_NCMetric.LoadDefaults();
        sizePair.m_Font.LoadValues(sizePair.m_NCMetric);

        return true;
    }
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
            CSchemePtr pScheme{};
            bool        bFound{false};
            if (sCurrentName.empty()) {
                sCurrentName = sParam[SchemeIndex];
            }
            if (!WinXPRegParseScheme(*this, pScheme, bFound, sCurrentName, sParam[SchemeIndex], sd.second)) {
                code = static_cast<HRESULT>(GetLastError());
                ReportError(Str::ElipsisW::Format(L"Scheme Load failed\r\n'%s'\r\n",
                    sCurrentName.c_str()), code, true, MB_ICONERROR);
                return IT_Invalid;
            }
            if (!bFound) {
                m_Schemes.emplace_back(std::move(pScheme));
            }
            else {
                code = static_cast<HRESULT>(0);
                ReportError(Str::ElipsisW::Format(L"Scheme Already Loaded\r\n'%s'\r\n",
                    sCurrentName.c_str()), code, true, MB_ICONWARNING);
            }
            IniStringW{}.swap(sCurrentName);
        }
    }
    return static_cast<int>(m_Schemes.size()) - 1;
}
