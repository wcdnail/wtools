#ifndef _wcdafx_curl_rest_h__
#define _wcdafx_curl_rest_h__

#ifdef _MSC_VER
#pragma once
#endif

#include "curl.api.h"
#include <string.hp.h>
#include <xmemory>
#include <boost/noncopyable.hpp>

namespace Curl
{
    class RestSession: boost::noncopyable
    {
    public:
        MYCURL_API RestSession();
        MYCURL_API ~RestSession();

        MYCURL_API void Reset();
        MYCURL_API bool SetProxy(char const* address, unsigned type, char const* cred) const;
        MYCURL_API bool SetVerbose(bool verbose) const;
        MYCURL_API bool SetTrace() const;
        MYCURL_API bool CheckConnection(CharString const& urlString) const;
        MYCURL_API bool Perform(CharString const& urlString, OStringStream& results) const;

    private:
        class Impl;
        std::auto_ptr<Impl> impl;
    };
}

#endif /* _wcdafx_curl_rest_h__ */
