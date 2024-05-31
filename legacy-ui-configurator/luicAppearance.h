#pragma once

#include "luicScheme.h"
#include "luicFontDef.h"
#include "luicPageImpl.h"
#include "luicThemePreviewer.h"
#include "luicSchemeManager.h"
#include "WTL/CColorButton.h"
#include <wcdafx.api.h>
#include <atlctrls.h>
#include <atlstr.h>

class CLUIApp;

struct CPageAppearance: CPageImpl
{
    DELETE_COPY_MOVE_OF(CPageAppearance);

    using ItemDef = CScheme::Item const&;
    using  String = CScheme::String;

    ~CPageAppearance() override;
    CPageAppearance(std::wstring&& caption);

    void NotifySchemesChanged();

private:
    enum MenuIDs
    {
        IDM_IMPORT_REGISTRY = 7327,
        IDM_IMPORT_WIN98THEME,
        IDM_IMPORT_WINXPREGFILE,
        IDM_CLEAR_SCHEMES,
    };

    CSchemePtr                        m_pSource;
    CScheme                        m_SchemeCopy;
    String                       m_sCurrentSize;
    bool                          m_bLoadValues;
    int                           m_nPrevScheme;
    int                             m_nPrevSize;
    WTL::CMenu                      m_mnuImport;
    WTL::CStatic                     m_stScheme;
    WTL::CComboBox                   m_cbScheme;
    WTL::CEdit                   m_edSchemeName;
    WTL::CStatic                m_stSchemeScale;
    WTL::CComboBox               m_cbSchemeSize;
    WTL::CButton                       m_bnSave;
    WTL::CButton                     m_bnImport;
    WTL::CButton                     m_bnRename;
    WTL::CButton                     m_bnDelete;
    WTL::CStatic                       m_stItem;
    WTL::CComboBox                     m_cbItem;
    WTL::CStatic                       m_stFont;
    WTL::CComboBox                     m_cbFont;
    WTL::CStatic                   m_stFontSize;
    WTL::CComboBox                 m_cbFontSize;
    WTL::CStatic                  m_stFontStyle;
    WTL::CButton                   m_bnFontBold;
    WTL::CButton                 m_bnFontItalic;
    WTL::CButton                 m_bnFontUndrln;
    WTL::CStatic                 m_stFontSmooth;
    WTL::CComboBox               m_cbFontSmooth;
    WTL::CButton              m_bcGradientCapts;
    WTL::CButton                  m_bcFlatMenus;
    CThemePreviewer                 m_stPreview;
    WTL::CStatic   m_stItemColor[IT_ColorCount];
    CColorButton   m_bnItemColor[IT_ColorCount];
    WTL::CStatic     m_stItemSize[IT_SizeCount];
    WTL::CEdit       m_edItemSize[IT_SizeCount];
    WTL::CUpDownCtrl m_udItemSize[IT_SizeCount];

    static void ImportMenuInit(WTL::CMenu& cMenu);
    void InitResizeMap();
    void CtlAdjustPositions();
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    void OnDestroy() override;

    void InitializeSchemes(CSchemeManager const& manager);
    void InitializeSizes();
    void InitializeItems();
    void InitializeFonts(FontMap const& fontsMap);
    void InitializeFontSizes();
    void InitializeFontSmooth();
    void InitializeFontButtons();
    void OnSchemesLoad(CLUIApp* pApp, int nInitialIndex);

    void SchemeEnable(BOOL bEnable);
    void ItemEnable(BOOL bEnable);
    void ItemClr1Enable(BOOL bEnable);
    void ItemClr2Enable(BOOL bEnable);
    void FontEnable(BOOL bEnable);
    void FontClr1Enable(BOOL bEnable);

    bool ItemColorSetBtn(int nButton, int iColor);
    void ItemColorSet(int nItem);
    void ItemSizeClear(int nSize);
    bool ItemSizeChanged(int nItem, int iSizeControl, bool bApply = false);

    void FontSetFamily(LOGFONT const& logFont);
    void FontSetSizes(LOGFONT const& logFont);
    bool ItemFontApplyChanges(int iFont, int iFontControl);
    bool ItemFontChanged(int nItem, int iFontControl = IT_Invalid, bool bApply = false);

    void OnItemSelect(int nItem);
    void OnSchemeSizeChanged();
    void OnSchemeSelected(CSchemePtr& pSource);

    void ColorPicker(int nButton);
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl) override;
    LRESULT OnNotify(int idCtrl, LPNMHDR pnmh) override;
    int ItemGetSel() const;
    void ItemColorTryChange(int nButton);
    void ApplyPendingChanges() const;
    void SchemeRenameShow(bool bShow);
};
