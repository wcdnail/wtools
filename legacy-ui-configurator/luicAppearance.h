#pragma once

#include "luicTheme.h"
#include "luicPageImpl.h"
#include "luicThemePreviewer.h"
#include "WTL/CColorButton.h"
#include <atlstr.h>
#include <atlctrls.h>

struct CTheme;

struct CPageAppearance: CPageImpl
{
    ~CPageAppearance() override;
    CPageAppearance(std::wstring&& caption);

private:
    friend struct CTheme;

    WTL::CStatic                      m_stTheme;
    WTL::CComboBox                    m_cbTheme;
    WTL::CStatic                  m_stThemeSize;
    WTL::CComboBox                m_cbThemeSize;
    WTL::CButton                  m_bnThemeSave;
    WTL::CButton                m_bnThemeImport;
    WTL::CButton                m_bnThemeRename;
    WTL::CButton                m_bnThemeDelete;
                           
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

    void InitResizeMap();
    void CtlAdjustPositions();
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    void OnDestroy() override;

    void ThemeEnable(BOOL bEnable);
    void ItemEnable(BOOL bEnable);
    void ItemClr1Enable(BOOL bEnable);
    void ItemClr2Enable(BOOL bEnable);
    void FontEnable(BOOL bEnable);
    void FontClr1Enable(BOOL bEnable);

    bool ItemColorSetBtn(int nButton, int iColor);
    void ItemColorSet(int nItem);
    void ItemSizeClear(int nSize);
    bool ItemSizeInit(int nItem, int nSize);

    void FontSetFamily(LOGFONT const* pLogFont);
    void FontSetSizes(LOGFONT const* pLogFont);
    void FontOnItemChaged(int nItem);

    void OnItemSelect(int nItem);
    void OnThemeSelect(int nThemeIndex);

    void ColorPicker(int nButton);
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl) override;
    LRESULT OnNotify(int idCtrl, LPNMHDR pnmh) override;
    int ItemGetSel(PCItemAssign& pAssignment) const;
    void ItemColorTryChange(int nButton);
};
