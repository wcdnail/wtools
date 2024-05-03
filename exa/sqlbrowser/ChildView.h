#pragma once

class CChildView : public CWnd
{
public:
	CChildView();
    virtual ~CChildView();

private:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};
