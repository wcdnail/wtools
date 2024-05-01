#pragma once
#if !defined(CONFIG_H)
#define CONFIG_H

#include "winver.h"

/* On Wine 8.0, the property sheet is sized incorrectly when using font sizes
 * other than 8.
 */
#if WINVER >= WINVER_7 && !defined(WINE)
#define DIALOG_FONTNAME "Segoe UI"
#define DIALOG_FONTSIZE 9
#else
#define DIALOG_FONTNAME "MS Shell Dlg"
#define DIALOG_FONTSIZE 8
#endif

/* Only build the classic style property sheet page and the en-US resources.
 */
#if !defined(MINIMAL) && (defined(WINE) || WINVER < WINVER_XP)
#define MINIMAL
#endif

/* Build support for specifying parameters.
 * Requires Windows XP or later.
 */
#if !defined(WITH_CMDLINE) && !defined(MINIMAL) && WINVER >= WINVER_XP
#define WITH_CMDLINE
#endif

/* Optionally display new scheme preview in addition to the classic one.
 * The dialog size on earlier versions is a bit too small for the new preview,
 * so it will display a hybrid between both versions.
 * The new preview displays all configurable elements, except for the menu
 * width (only applicable to maximized MDI child windows).
 */
#if !defined(WITH_NEW_PREVIEW)
#define WITH_NEW_PREVIEW
#endif

/* Default to a more compact modern preview if the preview window will be too
 * small for the "full" modern preview.
 */
#if !defined(WITH_NEW_PREVIEW_BIG) && defined(WITH_NEW_PREVIEW) && \
    DIALOG_FONTSIZE >= 9
#define WITH_NEW_PREVIEW_BIG
#endif

/* Preview context menu.
 * It allows switching preview modes.
 */
#if !defined(WITH_PREVIEW_CONTEXT_MENU) && defined(WITH_NEW_PREVIEW)
#define WITH_PREVIEW_CONTEXT_MENU
#endif

/* Support importing schemes from system applets.
 */
#if !defined(WITH_MIGRATION)
#define WITH_MIGRATION
#endif

/* Use undocumented Windows API calls to draw some preview elements in the
 * classic style preview.
 * They might be removed or work incorrectly in future versions.
 */
#if !defined(WITH_UNDOCUMENTED_API)
#define WITH_UNDOCUMENTED_API
#endif

/* On the classic style page, allow setting font sizes not preset in the combo
 * box.
 * This is the behaviour of the system applet prior to Vista.
 */
/*
#if !defined(WITH_EDITABLE_FONTSIZE)
#define WITH_EDITABLE_FONTSIZE
#endif
*/

/* On the classic style page, include controls for configuring:
 * Font underline, font angle, font smoothing,
 * title bar gradients and flat menu bars.
 * Makes the property sheet taller:
 * It requires a vertical screen resolution of 600 pixels instead of 480.
 */
#if !defined(WITH_CLASSIC_ADVANCED_SETTINGS) && WINVER >= WINVER_2K
#define WITH_CLASSIC_ADVANCED_SETTINGS
#endif

/* Themes page */
#if !defined(WITH_THEMES) && !defined(MINIMAL) && WINVER >= WINVER_8 && \
    defined(WITH_NEW_PREVIEW) && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#define WITH_THEMES
#endif

#endif  /* !defined(CONFIG_H) */
