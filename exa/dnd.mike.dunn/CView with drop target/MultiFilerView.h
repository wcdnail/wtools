#pragma once

struct IDropTargetHelper;

class CMultiFilerView : public CListView
{
protected: 
	CMultiFilerView();
	DECLARE_DYNCREATE(CMultiFilerView)

public:
	CMultiFilerDoc* GetDocument();

	//{{AFX_VIRTUAL(CMultiFilerView)
public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave();
protected:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

public:
	virtual ~CMultiFilerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CImageList         m_imglist;
    COleDropTarget     m_droptarget;

    IDropTargetHelper* m_piDropHelper;
    bool               m_bUseDnDHelper;

    BOOL ReadHdropData ( COleDataObject* pDataObject );

protected:
	//{{AFX_MSG(CMultiFilerView)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectAll();
	afx_msg void OnInvertSelection();
	afx_msg void OnRemoveFromList();
	afx_msg void OnClearList();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CMultiFilerDoc* CMultiFilerView::GetDocument() { return (CMultiFilerDoc*)m_pDocument; }
#endif
