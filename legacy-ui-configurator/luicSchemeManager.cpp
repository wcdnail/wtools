#include "stdafx.h"
#include "luicSchemeManager.h"
#include "luicRegistry.h"
#include "luicUtils.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>
#include <string.utils.format.h>
#include <3rd-party/inipp.h>
#include <functional>
#include <fstream>

static constexpr wchar_t REG_ClassicSchemes[] = L"Control Panel\\Appearance\\ClassicSchemes";

CSchemeManager::~CSchemeManager() = default;

CSchemeManager::CSchemeManager()
    : m_Schemes{}
{
}

template <typename ReturnType, typename SelfRef>
ReturnType& CSchemeManager::getSchemeRef(SelfRef& thiz, int index)
{
    if (index < 0 || index > static_cast<int>(thiz.m_Schemes.size()) - 1) {
        static ReturnType dummy{};
        DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
        return dummy;
    }
    return thiz.m_Schemes[index];
}

CSchemePtr const& CSchemeManager::operator[](int index) const
{
    return getSchemeRef<CSchemePtr const>(*this, index);
}

CSchemePtr& CSchemeManager::operator[](int index)
{
    return getSchemeRef<CSchemePtr>(*this, index);
}

bool CSchemeManager::FindOrCreate(String const& name, CSchemePtr& pointee) const
{
    for (auto const& it: m_Schemes) {
        if (it->Name() == name) {
            pointee = it;
            return true;
        }
    }
    pointee = std::make_shared<CScheme>(name);
    return false;
}

HRESULT CSchemeManager::Initialize()
{
    HRESULT           code{S_OK};
    SchemeVec  tempSchemes{};
    {
        auto pCurrent = std::make_shared<CScheme>(L"(Current)");
        if (!pCurrent->LoadDefaults()) {
            code = static_cast<HRESULT>(GetLastError());
            return code;
        }
        tempSchemes.emplace_back(std::move(pCurrent));
    }
    const CRegistry regClassics{HKEY_CURRENT_USER, REG_ClassicSchemes};
    if (!regClassics.IsOk()) {
        code = static_cast<HRESULT>(GetLastError());
        tempSchemes.swap(m_Schemes);
        return code;
    }
    const int nCount = regClassics.ForEachValue([&tempSchemes](HKEY hKey, PCWSTR szSchemename, int nLen) -> bool {
        const CRegistry regScheme{hKey, szSchemename};
        auto pScheme = std::make_shared<CScheme>(StrView{szSchemename, static_cast<size_t>(nLen)});
        if (!pScheme->LoadValues(regScheme)) {
            return false;
        }
        const int nSizeCount = regScheme.ForEachValue([&pScheme, &tempSchemes](HKEY hKey, PCWSTR szSizeName, int nLen) -> bool {
            const CRegistry regSizes{hKey, szSizeName};
            const String       sName{szSizeName, static_cast<size_t>(nLen)};
            if (!pScheme->LoadSizes(sName, regSizes)) {
                return false;
            }
            return true;
        });
        if (CRegistry::ResultFail == nSizeCount) {
            return false;
        }
        tempSchemes.emplace_back(std::move(pScheme));
        return true;
    });
    if (CRegistry::ResultFail == nCount) {
        code = static_cast<HRESULT>(GetLastError());
        const auto codeText = Str::ErrorCode<>::SystemMessage(code);
        DH::TPrintf(L"%s: WARNING: CRegistry::ForEachValue failed: %d '%s'\n", __FUNCTIONW__,
            code, codeText.GetString());
    }
    tempSchemes.swap(m_Schemes);
    return code;
}

namespace
{
    using     IniParser = inipp::Ini<char>;
    using     IniString = IniParser::String;
    using        IniMap = IniParser::Section;
    using    InutStream = std::basic_ifstream<char>;
    using SectionLoader = std::function<bool(CScheme& scheme, IniMap& iniFields)>;

    size_t IniStrToBinary(IniString const& str, LPBYTE data, size_t size)
    {
        if (str.empty()) {
            return 0;
        }
        size_t     count{0};
        size_t     begin{0};
        const size_t len{str.length()};
        while (count < size && begin < len) {
            auto end = str.find(' ', begin);
            if (IniString::npos == end) {
                end = len;
            }
            const BYTE value = std::stoi(str.substr(begin, end - begin));
            data[count] = value;
            ++count;
            begin = end + 1;
        }
        return count;
    }

    bool Ini98LoadMetrics(CScheme& scheme, IniMap& iniFields)
    {
        ICONMETRICSA iconMetric;
        ZeroMemory(&iconMetric, sizeof(iconMetric));
        auto dest{reinterpret_cast<LPBYTE>(&iconMetric)};
        size_t dstSize{sizeof(iconMetric)};
        if (IniStrToBinary(iniFields["IconMetrics"], dest, dstSize) != dstSize) {
            return false;
        }
        NONCLIENTMETRICSA ncMetrics;
        ZeroMemory(&ncMetrics, sizeof(ncMetrics));
        dest = reinterpret_cast<LPBYTE>(&ncMetrics);
        dstSize = sizeof(ncMetrics) - sizeof(ncMetrics.iPaddedBorderWidth);
        if (IniStrToBinary(iniFields["NonclientMetrics"], dest, dstSize) != dstSize) {
            return false;
        }
        WTL::CFont font(::CreateFontIndirectA(&iconMetric.lfFont));
        CSizePair& sizePair = scheme.GetSizesMap()[L"Default"];
        sizePair.m_Font[FONT_Desktop].Reset(font);
        sizePair.m_NCMetric.LoadValues(ncMetrics);
        sizePair.m_Font.LoadValues(sizePair.m_NCMetric);
        return true;
    }

    bool Ini98LoadColors(CScheme& scheme, IniMap& iniFields)
    {
        static const IniString clrName[] = {
          "Scrollbar"               // CLR_Scrollbar
        , "Background"              // CLR_Background
        , "ActiveTitle"             // CLR_ActiveTitle
        , "InactiveTitle"           // CLR_InactiveTitle
        , "Menu"                    // CLR_Menu
        , "Window"                  // CLR_Window
        , "WindowFrame"             // CLR_WindowFrame
        , "MenuText"                // CLR_MenuText
        , "WindowText"              // CLR_WindowText
        , "TitleText"               // CLR_TitleText
        , "ActiveBorder"            // CLR_ActiveBorder
        , "InactiveBorder"          // CLR_InactiveBorder
        , "AppWorkspace"            // CLR_AppWorkspace
        , "Hilight"                 // CLR_Hilight
        , "HilightText"             // CLR_HilightText
        , "ButtonFace"              // CLR_ButtonFace
        , "ButtonShadow"            // CLR_ButtonShadow
        , "GrayText"                // CLR_GrayText
        , "ButtonText"              // CLR_ButtonText
        , "InactiveTitleText"       // CLR_InactiveTitleText
        , "ButtonHilight"           // CLR_ButtonHilight
        , "ButtonDkShadow"          // CLR_ButtonDkShadow
        , "ButtonLight"             // CLR_ButtonLight
        , "InfoText"                // CLR_InfoText
        , "InfoWindow"              // CLR_InfoWindow
        , "ButtonAlternateFace"     // CLR_ButtonAlternateFace
        , "HotTrackingColor"        // CLR_HotTrackingColor
        , "GradientActiveTitle"     // CLR_GradientActiveTitle
        , "GradientInactiveTitle"   // CLR_GradientInactiveTitle
        , "MenuHilight"             // CLR_MenuHilight
        , "MenuBar"                 // CLR_MenuBar
        };
        for (int iColor = 0; iColor < _countof(clrName); iColor++) {
            COLORREF clrTemp = 0;
            auto const& it = iniFields.find(clrName[iColor]);
            if (it == iniFields.cend()) {
                clrTemp = GetSysColor(iColor); // TODO: get WIN98 pallete 
            }
            else if (3 != IniStrToBinary(it->second, reinterpret_cast<LPBYTE>(&clrTemp), sizeof(clrTemp))) {
                return false;
            }
            if (!scheme.GetColorPair(iColor).Reset(clrTemp)) {
                return false;
            }
        }
        return true;
    }
}

int CSchemeManager::LoadIni98(Path const& path)
{
    HRESULT         code{S_OK};
    const auto& pathname{path.native()};
    InutStream   fsInput{pathname};
    IniParser  iniParser{};
    if (!fsInput.is_open()) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"ERROR: open win98 theme FAILED!\r\n'%s'\r\n",
            pathname.c_str()), code, true);
        return IT_Invalid;
    }
    iniParser.parse(fsInput);

    struct SectionDef
    {
        std::string     name;
        SectionLoader loader;
    };
    static const SectionDef gs_Sections[] = {
        { "Metrics", Ini98LoadMetrics },
        { "Control Panel\\Colors", Ini98LoadColors },
    };
    CSchemePtr pScheme{};
    const bool  bFound{FindOrCreate(path.filename().native(), pScheme)};
    pScheme->SetGradientCaptions(false);
    pScheme->SetFlatMenus(false);
    for (auto const& sd: gs_Sections) {
        const auto it = iniParser.sections.find(sd.name);
        if (it == iniParser.sections.cend()) {
            code = static_cast<HRESULT>(ERROR_ACCESS_DENIED);
            ReportError(Str::ElipsisW::Format(L"ERROR: section '%S' is not found int win98 theme\r\n'%s'\r\n",
                sd.name.c_str(), pathname.c_str()), code, true, MB_ICONSTOP);
            return IT_Invalid;
        }
        auto& iniMap{it->second};
        if (!sd.loader(*pScheme, iniMap)) {
            code = static_cast<HRESULT>(ERROR_ACCESS_DENIED);
            ReportError(Str::ElipsisW::Format(L"ERROR: section '%S' is INVALID\r\n'%s'\r\n",
                sd.name.c_str(), pathname.c_str()), code, true, MB_ICONHAND);
            return IT_Invalid;
        }
    }
    if (!bFound) {
        m_Schemes.emplace_back(std::move(pScheme));
    }
    return static_cast<int>(m_Schemes.size()) - 1;
}
