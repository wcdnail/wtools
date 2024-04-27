#pragma once

#if defined(WCDAFX_DLL_BUILD_CF)
#   define WCDAFX_API __declspec(dllexport)
#elif defined(WCDAFX_DLL)
#   define WCDAFX_API __declspec(dllimport)
#else
#   define WCDAFX_API
#endif
