#include "stdafx.h"
#include "luicPageImpl.h"
#include "luicMain.h"
#include "UT/debug.assistance.h"

CPageImpl::~CPageImpl()
{
}

CPageImpl::CPageImpl(UINT idd)
    :         IDD(idd)
    , m_resiseMap()
{
}

HWND CPageImpl::CreateDlg(HWND hWndParent, LPARAM dwInitParam)
{
    return Super::Create(hWndParent, dwInitParam);
}

void CPageImpl::DlgResizeAdd(int nCtlID, DWORD dwResizeFlags)
{
    CWindow item = GetDlgItem(nCtlID);
    _ASSERT_EXPR(item.Detach() != nullptr, _CRT_WIDE("Attempting to append not existing control to ResizeMap!"));
    m_resiseMap.emplace_back(_AtlDlgResizeMap{ nCtlID, dwResizeFlags });
}

BOOL CPageImpl::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    UNREFERENCED_ARG(wndFocus);
    UNREFERENCED_ARG(lInitParam);
    m_resiseMap.emplace_back(_AtlDlgResizeMap{ -1, 0 });
    DlgResize_Init(false, false);
    return TRUE;
}

HBRUSH CPageImpl::OnEraseBkgnd(CDCHandle dc)
{
    return CLegacyUIConfigurator::g_brBackBrush.m_hBrush;
}

WTL::_AtlDlgResizeMap const* CPageImpl::GetDlgResizeMap() const
{
    static const WTL::_AtlDlgResizeMap emptyMap[] = {
        { -1, 0 }
    };
    return m_resiseMap.empty() ? emptyMap : &m_resiseMap[0];
}
