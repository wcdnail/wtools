#pragma once
#if !defined(THM_H)
#define THM_H

#include "config.h"

#if defined(WITH_THEMES)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

_Success_(return)
BOOL ApplyTheme(_In_ const TCHAR *filePath);

#endif  /* defined(WITH_THEMES) */
#endif  /* !defined(THM_H) */
