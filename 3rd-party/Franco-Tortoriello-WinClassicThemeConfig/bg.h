#pragma once
#if !defined(BG_H)
#define BG_H

#include "config.h"

#if defined(WITH_NEW_PREVIEW) || defined(WITH_THEMES)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#if defined(WITH_THEMES)

#define STRICT_TYPED_ITEMIDS
#include <ShlObj.h>

/* Desktop background image positions.
 * The numbers are the same than in Windows 8 .Theme files.
 */

#define BG_POS_CENTER   0  /* Center the image without changing its size */
#define BG_POS_TILE     1  /* Tile the image; if it is larger than the desktop,
                            * its top-left portion is shown */
#define BG_POS_STRETCH  2  /* Stretch the image to fill the desktop, without
                            * keeping its aspect ratio */
#define BG_POS_FIT      3  /* Expand or shrink the image to fit the desktop
                            * and show the entire image while keeping its
                            * aspect ratio; it may show the background in a
                            * pair of borders */
#define BG_POS_CROP     4  /* Expand or shrink and crop the image to fill the
                            * desktop, while keeping its aspect ratio */
#define BG_POS_SPAN     5  /* Extend the image across multiple monitors */

/* Desktop background image positions, as written to the registry and in XP-7
 * .Theme files (excluding Tiled as that is a different registry value)
 */
#define BG_POSREG_CENTER    0
#define BG_POSREG_CROP     10
#define BG_POSREG_FIT       6
#define BG_POSREG_STRETCH   2
#define BG_POSREG_SPAN     22

#endif  /* defined(WITH_THEMES) */

extern const TCHAR szDesktopKey[];
extern const TCHAR szPatternValue[];

#if defined(WITH_THEMES)
extern const TCHAR szTileValue[];
extern const TCHAR szWallpaperStyleValue[];
extern const TCHAR szWallpaperValue[];
#endif

_Success_(return != NULL)
BYTE *LoadCurrentDesktopPattern(void);

#if defined(WITH_THEMES)

_Success_(return)
BOOL ApplyBackgroundSettings(_In_ const TCHAR *szPattern, char bgStyle
#if WINVER >= WINVER_8
    , char autoColor
#endif
);

#if WINVER >= WINVER_8 && defined(UNICODE)
_Success_(return)
BOOL CreateIWallpaperInstance(void);
#endif

_Success_(return)
BOOL ApplyWallpaper(_In_ TCHAR *path);

#if WINVER >= WINVER_8 && defined(UNICODE)

_Success_(return > 0)
UINT GetScreenCount(void);

_Success_(return)
BOOL ApplyScreenWallpaper(_In_ const TCHAR *path, UINT screenIndex);

_Success_(return)
BOOL ApplySlideshowSettings(UINT intervalMilliseconds, BOOL bShuffle);

_Success_(return)
BOOL ApplySlideshow(_In_reads_(nPaths) TCHAR **paths, UINT nPaths);

#endif  /* WINVER >= WINVER_8 && defined(UNICODE) */

#endif  /* defined(WITH_THEMES) */

#endif  /* defined(WITH_NEW_PREVIEW) || defined(WITH_THEMES) */
#endif  /* !defined(BG_H) */
