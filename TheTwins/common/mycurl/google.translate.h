#ifndef _wcdafx_google_translate_h__
#define _wcdafx_google_translate_h__

#ifdef _MSC_VER
#pragma once
#endif 

#include "curl.api.h"
#include "curl.rest.h"
#include <string.hp.h>
#include <map>
#include <boost/noncopyable.hpp>

namespace Google
{
    class HttpTranslator: boost::noncopyable
    {
    public:
        typedef std::map<WidecharString, WidecharString> ResultPairs;

        MYCURL_API HttpTranslator();
        MYCURL_API ~HttpTranslator();

        Curl::RestSession& GetSession() { return Session; }

        MYCURL_API ResultPairs Translate(CharString const& text, char const* sourceLang, char const* targetLang) const;
        MYCURL_API ResultPairs Translate(WidecharString const& text, char const* sourceLang, char const* targetLang) const;

    private:
        Curl::RestSession Session;
    };
}

#endif // _wcdafx_google_translate_h__
