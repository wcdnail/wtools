#pragma once

class CSqlBrowser: public CWinAppEx
{
public:
	CSqlBrowser();
    virtual ~CSqlBrowser();

private:
	HMENU MdiMenu;
	HACCEL MdiAccel;

    virtual BOOL InitInstance();
    virtual int ExitInstance();
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();

	DECLARE_MESSAGE_MAP()
};

extern CSqlBrowser theApp;
