#pragma once

#if _MSC_VER >= 1200
#  define WINVER          0x0600
#  define _WIN32_WINNT    0x0600
#  define _WIN32_IE       0x0700
#else
#  define WINVER          0x0501
#  define _WIN32_WINNT    0x0501
#  define _WIN32_IE       0x0600
#  define _WIN32_WINDOWS  0x0410
#endif
