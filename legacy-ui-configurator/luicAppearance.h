#pragma once

#include "luicTheme.h"
#include "luicPageImpl.h"
#include "luicThemePreviewer.h"
#include "WTL/CColorButton.h"
#include <atlctrls.h>

struct CTheme;

struct CPageAppearance: CPageImpl
{
    ~CPageAppearance() override;
    CPageAppearance(std::wstring&& caption);

private:
    friend struct CTheme;

    WTL::CStatic                    m_stTheme;
    WTL::CComboBox                  m_cbTheme;
    WTL::CStatic                m_stThemeSize;
    WTL::CComboBox              m_cbThemeSize;
    WTL::CButton                m_bnThemeSave;
    WTL::CButton              m_bnThemeImport;
    WTL::CButton              m_bnThemeRename;
    WTL::CButton              m_bnThemeDelete;

    WTL::CStatic                     m_stItem;
    WTL::CComboBox                   m_cbItem;
    WTL::CStatic                m_stItemSize1;
    WTL::CEdit                  m_edItemSize1;
    WTL::CUpDownCtrl            m_udItemSize1;
    WTL::CStatic                m_stItemSize2;
    WTL::CEdit                  m_edItemSize2;
    WTL::CUpDownCtrl            m_udItemSize2;
    WTL::CStatic                 m_stItemClr1;
    WTL::CStatic                 m_stItemClr2;

    WTL::CStatic                     m_stFont;
    WTL::CComboBox                   m_cbFont;
    WTL::CStatic                 m_stFontSize;
    WTL::CComboBox               m_cbFontSize;
    WTL::CStatic                m_stFontWidth;
    WTL::CEdit                  m_edFontWidth;
    WTL::CUpDownCtrl            m_udFontWidth;
    WTL::CStatic                 m_stFontClr1;
    WTL::CStatic                m_stFontStyle;
    WTL::CButton                 m_bnFontBold;
    WTL::CButton               m_bnFontItalic;
    WTL::CButton               m_bnFontUndrln;
    WTL::CStatic                m_stFontAngle;
    WTL::CEdit                  m_edFontAngle;
    WTL::CUpDownCtrl            m_udFontAngle;
    WTL::CStatic               m_stFontSmooth;
    WTL::CComboBox             m_cbFontSmooth;

    WTL::CButton            m_bcGradientCapts;
    WTL::CButton                m_bcFlatMenus;
    CThemePreviewer               m_stPreview;

    CColorButton m_bnItemColor[IT_ColorCount];

    void InitResizeMap();
    void CtlAdjustPositions();
    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    void OnDestroy() override;

    void ThemeEnable(BOOL bEnable);
    void ItemEnable(BOOL bEnable);
    void ItemSize1Enable(BOOL bEnable);
    void ItemSize2Enable(BOOL bEnable);
    void ItemClr1Enable(BOOL bEnable);
    void ItemClr2Enable(BOOL bEnable);
    void FontEnable(BOOL bEnable);
    void FontClr1Enable(BOOL bEnable);

    bool ItemColorSetBtn(int nButton, int iColor);
    void ItemColorSet(int nItem);
    BOOL ItemSizeSet(int metric, int nSizeCtlID, WTL::CUpDownCtrl& udSize);

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
