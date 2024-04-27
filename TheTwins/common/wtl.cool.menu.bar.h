#pragma once

#if 0
#include <atlctrlw.h>
#include <atlgdi.h>

class CCoolMenuBar: public CCommandBarCtrlImpl<CCoolMenuBar>
{
private:
    typedef CCommandBarCtrlImpl<CCoolMenuBar> Super;

public:
    DECLARE_WND_SUPERCLASS(NULL, GetWndClassName())

    enum 
    {
        SidebarId = 666,
    };

    CCoolMenuBar();
    ~CCoolMenuBar();

    using Super::GetWndStyle;
    using Super::GetWndExStyle;
    using Super::GetWndCaption;
    using Super::Create;
    using Super::AttachMenu;
    using Super::LoadImages;
    using Super::operator HWND;

    void DrawItem(LPDRAWITEMSTRUCT di);
    void MeasureItem(LPMEASUREITEMSTRUCT mi);

private:
    friend class Super;

    CFont m_Font;

    void DrawMenuText(CDCHandle& dc, RECT& rc, LPCTSTR lpstrText, COLORREF colorText, COLORREF colorAccellerator);
    void DrawSideBar(CDCHandle& dc, RECT& grrect, LPCTSTR sidebarText);

	BEGIN_MSG_MAP(CCoolMenuBar)
		CHAIN_MSG_MAP(Super)
		ALT_MSG_MAP(1)
		CHAIN_MSG_MAP_ALT(Super, 1)
		ALT_MSG_MAP(2)
		CHAIN_MSG_MAP_ALT(Super, 2)
		ALT_MSG_MAP(3)
		CHAIN_MSG_MAP_ALT(Super, 3)
	END_MSG_MAP()
};
#endif
