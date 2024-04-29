#pragma once

#include "twins.langs.api.h"
#include "string.text.ids.h"
#include <string>
#include <map>
#include <boost/noncopyable.hpp>

#ifdef _WIN32
#  include <tchar.h>
#endif

namespace Twins
{
    class Strings: boost::noncopyable
    {
    public:
        _TWINS_LANG_API static Strings const& Instance();

        _TWINS_LANG_API std::string const& Get(unsigned id, char) const;
        _TWINS_LANG_API std::wstring const& Get(unsigned id, wchar_t) const;
        _TWINS_LANG_API std::string const& Null(char) const;
        _TWINS_LANG_API std::wstring const& Null(wchar_t) const;

        _TWINS_LANG_API void Select(unsigned id) const;

    private:
        typedef std::map<unsigned, std::string> AMap;
        typedef std::map<unsigned, std::wstring> WMap;
        typedef std::map<unsigned, AMap> ALangMap;
        typedef std::map<unsigned, WMap> WLangMap;

        std::string ANull;
        std::wstring WNull;
        mutable ALangMap ALang;
        mutable WLangMap WLang;
        mutable unsigned LangId;

        ALangMap& GetLangMap(char) const;
        WLangMap& GetLangMap(wchar_t) const;

        template <typename C, typename M> 
        typename M& GetCurrentMap(unsigned id) const;

        template <typename C> 
        std::basic_string<C> const& FindOrLoad(unsigned id) const;

    private:
        Strings();
        ~Strings();
    };

    template <typename C> 
    inline std::basic_string<C> const& _GetString(unsigned id)
    {
        return Strings::Instance().Get(id, (C)0);
    }
}

#define _L(Id)      Twins::_GetString<wchar_t>(Id)
#define _LS(Id)     _L(Id).c_str()

#define _LA(Id)     Twins::_GetString<char>(Id)
#define _LSA(Id)    _LA(Id).c_str()

#ifdef _WIN32
#  define _LT(Id)   Twins::_GetString<TCHAR>(Id)
#  define _LTS(Id)  _LT(Id).c_str()
#endif
