#ifndef _wcdafx_curl_api_h__
#define _wcdafx_curl_api_h__

#ifdef _MSC_VER
#pragma once
#endif

#if defined(MYCURL_DLL_BUILD)
#   define MYCURL_API __declspec(dllexport)
#elif defined(MYCURL_DLL)
#   define MYCURL_API __declspec(dllimport)
#else
#   define MYCURL_API
#endif

#endif /* _wcdafx_curl_api_h__ */
