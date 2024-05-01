#pragma once
#if !defined(SSAVE_H)
#define SSAVE_H

#include "config.h"

#if defined(WITH_THEMES)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern TCHAR szScreenSaverValue[];

_Success_(return)
BOOL ApplyScreenSaver(_In_ const TCHAR *path);

#endif  /* defined(WITH_NEW_PREVIEW) || defined(WITH_THEMES) */
#endif  /* !defined(SSAVE_H) */
