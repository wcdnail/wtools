#pragma once
#if !defined(CLASSTYL_H)
#define CLASSTYL_H

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/* Maximum lenghts including the terminating NULL char */
#define SCHEMENAME_MAX_SIZE  29
#define SIZENAME_MAX_SIZE    14

extern const TCHAR szSchemesKey[];

extern const TCHAR szColorsKey[];

#if WINVER >= WINVER_XP
extern const TCHAR szFlatMenusValue[];
#endif

enum size
{
    SIZE_BORDER,
    SIZE_SCROLLWIDTH,
    SIZE_SCROLLHEIGHT,
    SIZE_CAPTIONWIDTH,
    SIZE_CAPTIONHEIGHT,
    SIZE_SMCAPTIONWIDTH,
    SIZE_SMCAPTIONHEIGHT,
    SIZE_MENUWIDTH,
    SIZE_MENUHEIGHT,
#if WINVER >= WINVER_VISTA
    SIZE_PADDEDBORDER,
#endif
    NUM_SIZES
};

enum font
{
    FONT_CAPTION,
    FONT_SMCAPTION,
    FONT_MENU,
    FONT_TOOLTIP,
    FONT_MESSAGE,
    FONT_DESKTOP,
    NUM_FONTS
};

#if WINVER < WINVER_2K
#define NUM_COLORS 25
#elif WINVER < WINVER_XP
#define NUM_COLORS 29
#else
#define NUM_COLORS 31
#endif

#if WINVER >= WINVER_2K
#define COLOR_ALTERNATEBTNFACE 25
#endif

#if WINVER < WINVER_2K
#define NUM_COLORS_LEGACY NUM_COLORS
#else
#define NUM_COLORS_LEGACY 29
#endif

typedef struct tagSCHEME_DATA
{
    NONCLIENTMETRICS ncMetrics;
    LOGFONT lfIconFont;

    COLORREF colors[NUM_COLORS];

#if defined(WITH_NEW_PREVIEW) || defined(WITH_THEMES)
    BYTE *desktopPattern;
#endif

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    /* These fields correspond to
     * HKCU\Control Panel\Desktop\UserPreferencesMask
     */
#if WINVER >= WINVER_2K
    BOOL bGradientCaptions;
#endif
#if WINVER >= WINVER_XP
    BOOL bFlatMenus;
#endif
#endif
} SCHEME_DATA;

/* A scheme consists of a color palette and a list of sizes.
 * Only the names are in RAM; the actual data is lazy-loaded from the registry.
 */
typedef struct tagSCHEME
{
    struct tagSCHEME *next;
    struct tagSCHEME *sizes;
    TCHAR *name;
} SCHEME;

typedef struct tagSCHEME_SELECTION
{
    SCHEME *color;
    SCHEME *size;
} SCHEME_SELECTION;

typedef struct tagSCHEMES_DATA
{
    SCHEME *schemes;
    SCHEME_SELECTION selected;

    SCHEME_DATA scheme;
} SCHEMES_DATA;

extern SCHEMES_DATA *g_schemes;

extern const TCHAR *g_colorNames[NUM_COLORS];
extern const TCHAR *g_sizeNames[NUM_SIZES];
extern const TCHAR *g_fontNames[NUM_FONTS];

typedef struct tagSIZE_RANGE
{
    int min;
    int max;
    int current;  /* saved size */
} SIZE_RANGE;

extern SIZE_RANGE g_sizeRanges[NUM_SIZES];

_Success_(return)
BOOL LoadCurrentScheme(_Out_ SCHEME_DATA *pData);

_Success_(return)
BOOL RefreshCurrentColors(void);

_Success_(return)
BOOL RefreshCurrentNcMetrics(void);

_Success_(return)
BOOL RefreshCurrentIconFont(void);

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_2K
_Success_(return)
BOOL RefreshCurrentGradientCaptionsSetting(void);
#endif

#if defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_XP
_Success_(return)
BOOL RefreshCurrentFlatMenusSetting(void);
#endif

void LoadSchemesList(void);

_Ret_maybenull_
SCHEME *FindScheme(_In_ SCHEME *pSchemes, _In_ const TCHAR *name);

_Success_(return)
BOOL GetActiveScheme(_In_ SCHEME *pSchemes, _Out_ SCHEME_SELECTION *pSelection);

_Success_(return != NULL)
SCHEME *GetActiveSchemeSize(_In_ SCHEME *pScheme);

_Success_(return)
BOOL LoadScheme(
    _Out_ SCHEME_DATA *pData, _In_ const SCHEME_SELECTION *pSelection);

_Success_(return)
BOOL LoadSchemeSize(
    _Out_ SCHEME_DATA *pData, _In_ const SCHEME_SELECTION *pSelection);

_Success_(return >= 0)
int GetSchemeMetric(_In_ const NONCLIENTMETRICS *ncMetrics, int metric);

_Success_(return >= 0)
int SetSchemeMetric(_Out_ NONCLIENTMETRICS *ncMetrics, int metric, int value);

_Ret_maybenull_
LOGFONT *GetSchemeFont(_In_ SCHEME_DATA *pData, int metric);

void UpdateMinHeightFromFont(_In_ const LOGFONT *plfFont, int iSize);

void ApplyScheme(
    _Inout_ SCHEME_DATA *pData, _In_opt_ const SCHEME_SELECTION *pSelection);

_Ret_maybenull_
SCHEME *CreateScheme(_In_ const TCHAR *name);

_Success_(return)
BOOL SaveSchemeColors(_In_ SCHEME_DATA *pData, _In_ const TCHAR *schemeName);

_Success_(return)
BOOL SaveSchemeSize(_In_ SCHEME_DATA *pScheme,
    _In_ const TCHAR *schemeName, _In_ const TCHAR * sizeName);

void SaveSchemeSelection(
    _In_ const TCHAR *schemeName, _In_ const TCHAR *sizeName);

_Success_(return)
BOOL RenameScheme(_Inout_ SCHEME *pScheme, _In_ const TCHAR *newName);

_Success_(return)
BOOL RenameSchemeSize(
    _In_ SCHEME *pScheme, _Inout_ SCHEME *pSize, _In_ const TCHAR *newSizeName);

_Success_(return)
BOOL DeleteSchemeFromList(_Inout_ SCHEME **ppList, _Inout_ SCHEME **ppScheme);

_Success_(return)
BOOL DeleteScheme(_Inout_ SCHEME **ppScheme);

_Success_(return)
BOOL DeleteSize(_Inout_ SCHEME **ppScheme, _Inout_ SCHEME **ppSize);

void FreeScheme(_Inout_ SCHEME **ppScheme);

void FreeSchemes(_Inout_ SCHEME **ppSchemes);

#if defined(WITH_CMDLINE) || defined(WITH_THEMES)
void SetWin3Style(_Inout_ SCHEME_DATA *pData);

void SetWin16Metrics(_Out_ SCHEME_DATA *pData);
#endif

#endif  /* !defined(CLASSTYL_H) */
