// ���� �������� ��� MFC Samples ������������� ���������������� ����������������� ���������� Fluent �� ������ MFC � Microsoft Office
// ("Fluent UI") � ��������������� ������������� ��� ���������� �������� � �������� ���������� �
// ����������� �� ������ Microsoft Foundation Classes � ��������� ����������� ������������,
// ���������� � ����������� ����������� ���������� MFC C++. 
// ������� ������������� ���������� �� �����������, ������������� ��� ��������������� Fluent UI �������� ��������. 
// ��� ��������� �������������� �������� � ����� ������������ ��������� Fluent UI �������� ���-����
// http://msdn.microsoft.com/officeui.
//
// (C) ���������� ���������� (Microsoft Corp.)
// ��� ����� ��������.

// view.h : ��������� ������ CMinView
//

#pragma once

class CCntrItem;

class CMinView : public CView
{
protected: // ������� ������ �� ������������
	CMinView();
	DECLARE_DYNCREATE(CMinView)

// ��������
public:
	CDoc* GetDocument() const;
	// m_pSelection ����������� ���������� ������� � ������� CCntrItem.
	// ��� ������ ����������, ����� ����������-���� �� ���������� ���
	//  ������������� ���������� �������, �������� � ������ ��������� �� ������� �������� ���
	//  ��������� ��������, ������� �� �������� ��������� CCntrItem. ������ ������ ���������
	//  ��������������� � ����� ������ ����������

	// TODO: �������� ������ ������ ��������� � ����� ����������
	CCntrItem* m_pSelection;

// ��������
public:

// ���������������
public:
	virtual void OnDraw(CDC* pDC);  // �������������� ��� ��������� ����� �������������
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ���������� � ������ ��� ����� ������������
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL IsSelected(const CObject* pDocItem) const;// ��������� ����������

// ����������
public:
	virtual ~CMinView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ��������� ������� ����� ���������
protected:
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	afx_msg void OnFilePrint();
	afx_msg void OnCancelEditSrvr();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // ���������� ������ � view.cpp
inline CDoc* CMinView::GetDocument() const
   { return reinterpret_cast<CDoc*>(m_pDocument); }
#endif

