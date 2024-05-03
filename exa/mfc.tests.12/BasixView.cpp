#include "stdafx.h"
#ifndef SHARED_HANDLERS
#include "basix.h"
#endif
#include "BasixDoc.h"
#include "BasixView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CBasixView, CView)

BEGIN_MESSAGE_MAP(CBasixView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

CBasixView::CBasixView()
{
}

CBasixView::~CBasixView()
{
}

BOOL CBasixView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

void CBasixView::OnDraw(CDC* /*pDC*/)
{
	CBasixDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}

void CBasixView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CBasixView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

#ifdef _DEBUG
void CBasixView::AssertValid() const
{
	CView::AssertValid();
}

void CBasixView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBasixDoc* CBasixView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBasixDoc)));
	return (CBasixDoc*)m_pDocument;
}
#endif //_DEBUG

