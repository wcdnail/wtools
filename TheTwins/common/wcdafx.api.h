#ifndef _wcdafx_common_api_h__
#define _wcdafx_common_api_h__

#ifdef _MSC_VER
#pragma once
#endif

#if defined(WCDAFX_DLL_BUILD_CF)
#   define WCDAFX_API __declspec(dllexport)
#elif defined(WCDAFX_DLL)
#   define WCDAFX_API __declspec(dllimport)
#else
#   define WCDAFX_API
#endif

#endif /* _wcdafx_common_api_h__ */
