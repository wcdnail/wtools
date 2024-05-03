#pragma once

class CBasixView : public CView
{
protected:
	CBasixView();
	DECLARE_DYNCREATE(CBasixView)

public:
    virtual ~CBasixView();

	CBasixDoc* GetDocument() const;

	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CBasixDoc* CBasixView::GetDocument() const
{ 
    return reinterpret_cast<CBasixDoc*>(m_pDocument); 
}
#endif

