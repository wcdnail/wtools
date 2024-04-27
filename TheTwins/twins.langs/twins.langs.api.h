#pragma once

#if defined(_BUILD_TWINS_LANG)
#   define _TWINS_LANG_API __declspec(dllexport)
#else
#   define _TWINS_LANG_API
#endif
