#include "stdafx.h"
#include "luicFontDef.h"
#include "dh.tracing.h"

namespace
{
struct EnumFontProcArgs
{
    int  m_Counter = {0};
    HRESULT m_Code = {0};
    CDC m_ScreenDC = {};
    FontMap  m_Map = {};

    bool Parse(ENUMLOGFONTEXW const* lpelfe, NEWTEXTMETRICEX const* lpntme)
    {
        FontStr    faceName = lpelfe->elfLogFont.lfFaceName;
        CharsetPair charSet = std::make_pair<int, FontStr>(lpelfe->elfLogFont.lfCharSet, lpelfe->elfScript);
        CFontDef*   fontDef = nullptr;
        auto it = m_Map.find(faceName);
        if (it == m_Map.end()) {
            fontDef = &m_Map[faceName];
            fontDef->m_sFullName = lpelfe->elfFullName;
            fontDef->m_tmHeight = lpntme->ntmTm.tmHeight;
        }
        else {
            fontDef = &it->second;
        }
        if (!fontDef->m_sCharset.contains(charSet)) {
            fontDef->m_sCharset.emplace(charSet);
        }
        ++m_Counter;
        return true;
    }
};
}

static int CALLBACK FontEnumerator(ENUMLOGFONTEXW const*  lpelfe,
                                   NEWTEXTMETRICEX const* lpntme,
                                   DWORD dwFontType,
                                   EnumFontProcArgs& args)
{
    UNREFERENCED_PARAMETER(dwFontType);
#ifdef _DEBUG_XTRA
    uint64_t signCodePage = *reinterpret_cast<const uint64_t*>(lpntme->ntmFontSig.fsCsb);
    DebugPrintf(L">> %4d t:%04X ^%-3d csb:%p >>cs:%-3d<< %40s [fn:'%s' scr:'%s']\n", args.m_Counter + 1, dwFontType,
        lpntme->ntmTm.tmHeight,
        signCodePage,
        lpelfe->elfLogFont.lfCharSet,
        lpelfe->elfLogFont.lfFaceName,
        lpelfe->elfFullName,
        lpelfe->elfStyle
    );
#endif
    if (!args.Parse(lpelfe, lpntme)) {
        return 0;
    }
    return 1;
}

HRESULT CFontDef::LoadAll(FontMap& target)
{
    EnumFontProcArgs args{ 0, S_OK, {GetDC(nullptr)}, {} };
    if (!args.m_ScreenDC.m_hDC) {
        args.m_Code = static_cast<HRESULT>(GetLastError());
        return args.m_Code;
    }
    WTL::CLogFont lfFont;
    lfFont.lfCharSet = DEFAULT_CHARSET;
    lfFont.lfFaceName[0] = TEXT('\0');
    lfFont.lfPitchAndFamily = DEFAULT_PITCH;
    auto  proc = reinterpret_cast<FONTENUMPROC>(FontEnumerator);
    auto param = reinterpret_cast<LPARAM>(&args);
    if (EnumFontFamiliesExW(args.m_ScreenDC, &lfFont, proc, param, 0) > 0) {
        target.swap(args.m_Map);
    }
    return args.m_Code;
}
