#pragma once

#include <afxoutlookbar.h>

class CSqlBrowser;

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)

public:
	CMainFrame(CSqlBrowser& app);
    virtual ~CMainFrame();

private:
    enum 
    {
        IdOutlookBar = 10001
    };

    CSqlBrowser& AppRef;
    UINT LookId;
	CMFCRibbonBar RibbonBar;
	CMFCRibbonApplicationButton MainButton;
    CMFCOutlookBar OutlookBar;
	CMFCToolBarImages PanelImages;
	CMFCRibbonStatusBar StatusBar;

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};
