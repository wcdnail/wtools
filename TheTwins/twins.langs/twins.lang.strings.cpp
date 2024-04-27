#include "twins.lang.strings.h"
#include <wtypes.h>
#include <atlstr.h>

extern "C" HMODULE MyInstance;

namespace Twins
{
    Strings const& Strings::Instance()
    {
        static Strings instance;
        return instance;
    }

    Strings::Strings()
        : ANull("<NIL>")
        , WNull(L"<NIL>")
        , ALang()
        , WLang()
        , LangId(MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT))
    {}

    Strings::~Strings()
    {}

    void Strings::Select(unsigned id) const { LangId = id; }

    std::string const& Strings::Null(char) const { return ANull; }
    std::wstring const& Strings::Null(wchar_t) const { return WNull; }
    Strings::ALangMap& Strings::GetLangMap(char) const { return ALang; }
    Strings::WLangMap& Strings::GetLangMap(wchar_t) const { return WLang; }

    template <typename CharType, typename MsgType> 
    MsgType& Strings::GetCurrentMap(unsigned id) const
    {
        typedef std::map<unsigned, MsgType> LangMapType;
        LangMapType& map = GetLangMap(static_cast<CharType>(0));
        auto it = map.find(id);
        if (it != map.end()) {
            return it->second;
        }
        MsgType newm;
        auto pb = map.emplace(std::make_pair(id, newm));
        return pb.first->second;
    }

    template <typename CharType> 
    std::basic_string<CharType> const& Strings::FindOrLoad(unsigned id) const
    {
        typedef std::map<unsigned, std::basic_string<CharType> > MapType;

        MapType& map = GetCurrentMap<CharType, MapType>(LangId);
        typename MapType::const_iterator it = map.find(id);
        if (it != map.end())
            return it->second;
        
        CStringT< CharType, StrTraitATL< CharType, ChTraitsCRT< CharType > > > temp;
        if (temp.LoadString(MyInstance, id, LangId))
        {
            std::basic_string<CharType> item = (CharType const*)temp;
            auto pb = map.emplace(std::make_pair(id, item));
            return pb.second ? pb.first->second : Null(static_cast<CharType>(0));
        }
        
        return Null(static_cast<CharType>(0));
    }

    std::string const& Strings::Get(unsigned id, char) const
    {
        return FindOrLoad<char>(id);
    }

    std::wstring const& Strings::Get(unsigned id, wchar_t) const
    {
        return FindOrLoad<wchar_t>(id);
    }
}
