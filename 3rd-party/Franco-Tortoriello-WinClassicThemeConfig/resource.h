#pragma once
#if !defined(RESOURCE_H)
#define RESOURCE_H

#include "config.h"

#define VERSION      1,5,4,0
#define VERSION_STR  "1.5.4\0"

#define SCHEMEPREVIEWWND_CLASS "SchemePreviewWndClass"

/* String constants */

#define NEWLINE "\r\n"

/* Internal names for command line usage */
#define PAGENAME_CLASSIC  L"Appearance"
#if !defined(MINIMAL)
#define PAGENAME_EFF      L"Effects"
#endif

/* Resource IDs */

/* Icons */
#define IDI_PROGRAM     100
#define IDI_SAVEICON    101
#define IDI_RENAMEICON  102
#define IDI_DELICON     103

/* Preview menu */
#define IDM_PREVIEW_MENU  100
#define IDM_NORMAL        101
#define IDM_DISABLED      102
#define IDM_SELECTED      103

#if defined(WITH_PREVIEW_CONTEXT_MENU)
/* Preview window context menu */
#define IDM_PREVIEW_CONTEXT_MENU  110
#if defined(WITH_NEW_PREVIEW)
#define IDM_PREVIEW_CLASSIC       111
#endif
#endif

/* Dialogs */
#define IDD_CLASSIC         200
#define IDD_CLASSIC_SAVE    201
#define IDD_CLASSIC_RENAME  202
#if !defined(MINIMAL)
#define IDD_EFF             500
#define IDD_EFF_ANIM        501
#endif

/* Controls */

#define IDC_STATIC  -1

/* Classic Visual Style page */
#define IDC_CLASSIC_PREVIEW           100
#define IDC_CLASSIC_SCHEME            101
#define IDC_CLASSIC_SCHEMESIZE        102
#define IDC_CLASSIC_SAVE              103
#define IDC_CLASSIC_RENAME            104
#define IDC_CLASSIC_DELETE            105
#define IDC_CLASSIC_ELEMENT           106
#define IDC_CLASSIC_SIZE1_T           107
#define IDC_CLASSIC_SIZE1_E           108
#define IDC_CLASSIC_SIZE1_UD          109
#define IDC_CLASSIC_SIZE2_T           110
#define IDC_CLASSIC_SIZE2_E           111
#define IDC_CLASSIC_SIZE2_UD          112
#define IDC_CLASSIC_COLOR1_T          113
#define IDC_CLASSIC_COLOR1            114
#define IDC_CLASSIC_COLOR2_T          115
#define IDC_CLASSIC_COLOR2            116
#define IDC_CLASSIC_FONTNAME_T        117
#define IDC_CLASSIC_FONTNAME          118
#define IDC_CLASSIC_FONTSIZE_T        119
#define IDC_CLASSIC_FONTSIZE          120
#define IDC_CLASSIC_FONTWIDTH_T       121
#define IDC_CLASSIC_FONTWIDTH_E       122
#define IDC_CLASSIC_FONTWIDTH_UD      123
#define IDC_CLASSIC_FONTCOLOR_T       124
#define IDC_CLASSIC_FONTCOLOR         125
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#define IDC_CLASSIC_FONTANGLE_T       126
#define IDC_CLASSIC_FONTANGLE_E       127
#define IDC_CLASSIC_FONTANGLE_UD      128
#endif
#define IDC_CLASSIC_FONTSTYLE_T       129
#define IDC_CLASSIC_FONTBOLD          130
#define IDC_CLASSIC_FONTITALIC        131
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#define IDC_CLASSIC_FONTUNDERLINE     132
#endif
#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#define IDC_CLASSIC_FONTSMOOTHING_T   133
#define IDC_CLASSIC_FONTSMOOTHING     134
#endif
#define IDC_CLASSIC_NEWNAME           135
#define IDC_CLASSIC_NEWSIZE           136
#if WINVER >= WINVER_2K && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#define IDC_CLASSIC_GRADIENTCAPTIONS  137
#endif
#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#define IDC_CLASSIC_FLATMENUS         138
#endif

#if !defined(MINIMAL)

/* Advanced Settings page */
#define IDC_EFF_ICONS_SIZE_E        100
#define IDC_EFF_ICONS_SIZE_UD       101
#define IDC_EFF_ICONS_XMARGIN_E     102
#define IDC_EFF_ICONS_XMARGIN_UD    103
#define IDC_EFF_ICONS_YMARGIN_E     104
#define IDC_EFF_ICONS_YMARGIN_UD    105
#if WINVER >= WINVER_XP
#define IDC_EFF_ICONS_LABELSHADOW   106
#endif
#if WINVER >= WINVER_VISTA
#define IDC_EFF_ICONS_TRANSSELRECT  107
#define IDC_EFF_ICONS_CUSTOMIZE     108
#endif
#if WINVER >= WINVER_2K
#define IDC_EFF_MASTER              109
#endif
#define IDC_EFF_ANIMATIONS          110
#if WINVER >= WINVER_XP
#define IDC_EFF_MENUSHADOW          111
#define IDC_EFF_POINTERSHADOW       112
#endif
#if WINVER >= WINVER_2K
#define IDC_EFF_DRAGFULLWIN         113
#endif
#if WINVER >= WINVER_2K && !defined(WINVER_IS_98)
#define IDC_EFF_FONTSMOOTHING       114
#endif
#if WINVER >= WINVER_XP
#define IDC_EFF_FONTSMOOTHING_TYPE  115
#endif
#if WINVER >= WINVER_2K
#define IDC_EFF_KEYBOARDCUES        116
#if !defined(WINVER_IS_98)
#define IDC_EFF_RIGHTALIGNMENU      117
#endif
#define IDC_EFF_HIGHCONTRAST        118
#endif  /* WINVER >= WINVER_2K */
#define IDC_EFF_RESET               119

#define IDC_ANIM_MENUOPEN       140
#define IDC_ANIM_MENUOPEN_TYPE  141
#define IDC_ANIM_MENUSEL        142
#define IDC_ANIM_TOOLTIP        143
#define IDC_ANIM_TOOLTIP_TYPE   144
#define IDC_ANIM_COMBOBOX       145
#define IDC_ANIM_LISTBOX        146
#define IDC_ANIM_WINDOW         147
#if WINVER >= WINVER_VISTA
#define IDC_ANIM_CONTROL        148
#endif

#endif  /* !defined(MINIMAL) */

/* Strings */

#define IDS_PROPSHEET_NAME                 41
/* For displaying in the Control Panel */
#define IDS_APP_DESCRIPTION                42
#if WINVER >= WINVER_VISTA
#define IDS_CLASSIC                        45
#define IDS_CLASSIC_KEYWORDS               46
#if !defined(MINIMAL)
#define IDS_EFF                            51
#define IDS_EFF_KEYWORDS                   52
#endif
#endif

#define IDS_OK                             50

#define IDS_ERROR                          60
#define IDS_ERROR_GENERIC                  61
#define IDS_ERROR_MEM                      62

#define IDS_CURRENT_ELEMENT                70

/* Preview window */
#define IDS_INACTIVE_CAPTION              100
#define IDS_ACTIVE_CAPTION                101
#define IDS_WINDOW_TEXT                   102
#define IDS_MESSAGE_CAPTION               103
#define IDS_MESSAGE_TEXT                  104
#define IDS_MAIN_CAPTION                  105
#define IDS_DISABLED_TEXT                 106
#define IDS_NORMAL_TEXT                   107
#define IDS_SELECTED_TEXT                 108
#define IDS_PALETTE_CAPTION               109
#define IDS_BUTTON_TEXT                   110
#define IDS_TOOLTIP_TEXT                  111
#define IDS_ICON_LABEL                    112

/* Classic Visual Style elements */
/* Note: Keep in sync with wndclas.h */
#define IDS_ELEMENT_0                     200
#define IDS_ELEMENT_1                     201
#define IDS_ELEMENT_2                     202
#define IDS_ELEMENT_3                     203
#define IDS_ELEMENT_4                     204
#define IDS_ELEMENT_5                     205
#define IDS_ELEMENT_6                     206
#define IDS_ELEMENT_7                     207
#define IDS_ELEMENT_8                     208
#define IDS_ELEMENT_9                     209
#define IDS_ELEMENT_10                    210
#define IDS_ELEMENT_11                    211
#define IDS_ELEMENT_12                    212
#define IDS_ELEMENT_13                    213
#define IDS_ELEMENT_14                    214
#define IDS_ELEMENT_15                    215
#define IDS_ELEMENT_16                    216
#if WINVER >= WINVER_2K
#define IDS_ELEMENT_17                    217
#define IDS_HYPERLINK_TEXT     IDS_ELEMENT_17
#endif
#if WINVER >= WINVER_XP
#define IDS_ELEMENT_18                    218
#endif
#if WINVER >= WINVER_VISTA
#define IDS_ELEMENT_19                    219
#endif

#define IDS_BOLD                          230
#define IDS_ITALIC                        231
#if defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#define IDS_UNDERLINE                     232
#endif

#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
#define IDS_FONTSMOOTHING_DEFAULT         233
#define IDS_FONTSMOOTHING_OFF             234
#define IDS_FONTSMOOTHING_CT              235
#define IDS_FONTSMOOTHING_CTN             236
#endif

/* Save/Rename/Delete Scheme dialogs */
#define IDS_SAVESCHEME                    240
#define IDS_RENAMESCHEME                  241
#define IDS_DELETESCHEME                  242
#define IDS_SAVESCHEME_ERROR              243
#define IDS_RENAMESCHEME_ERROR            244
#define IDS_RENAMESIZE_ERROR              245
#define IDS_SAVESCHEME_CONFIRM_OVERWRITE  246
#define IDS_RENAMESCHEME_CONFLICT         247
#define IDS_RENAMESIZE_CONFLICT           248
#define IDS_SAVESCHEME_NO_NAME            249
#define IDS_SAVESCHEME_NO_SIZE            250
#define IDS_SAVESCHEME_DEFAULT_NAME       251
#define IDS_SAVESCHEME_DEFAULT_SIZE       252
#define IDS_DELETE_CONFIRM_TITLE          253
#define IDS_DELETESCHEME_CONFIRM_SCHEME   254
#define IDS_DELETESCHEME_CONFIRM_SIZE     255
#define IDS_DELETESCHEME_ERROR_SCHEME     256
#define IDS_DELETESCHEME_ERROR_SIZE       257

/* Effects dialogs */
#if !defined(MINIMAL) && WINVER >= WINVER_2K
#define IDS_ANIM_SLIDE                    500
#define IDS_ANIM_FADE                     501
#if WINVER >= WINVER_XP
#define IDS_EFF_MENUSHADOW_TT             502
#endif
#define IDS_EFF_KEYBOARDCUES_TT           503
#if !defined(WINVER_IS_98)
#define IDS_EFF_RIGHTALIGNMENU_TT         504
#endif
#define IDS_EFF_HIGHCONTRAST_TT           505
#if WINVER >= WINVER_VISTA
#define IDS_ANIM_TYPE_TT                  506
#endif
#endif

/* Command line messages */
#if defined(WITH_CMDLINE)
#define IDS_USAGE_GENERAL                1000
#define IDS_USAGE_APPEARANCE             1002
#define IDS_USAGE_EFF                    1005

#if defined(WITH_THEMES)
#define IDS_THEME_INVALID                1100
#endif

#define IDS_SCHEME                       1200
#define IDS_SIZES                        1201
#define IDS_SCHEME_LOAD_ERROR            1202
#define IDS_NO_SCHEMES                   1203
#endif

#endif  /* !defined(RESOURCE_H) */
