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

// cntritem.cpp : ���������� ������ CCntrItem
//

#include "stdafx.h"
#include "cm.app.h"

#include "doc.h"
#include "view.h"
#include "cntritem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���������� CCntrItem

IMPLEMENT_SERIAL(CCntrItem, COleClientItem, 0)

CCntrItem::CCntrItem(CDoc* pContainer)
	: COleClientItem(pContainer)
{
	// TODO: �������� ��� ��� ������������ ������ ������������
}

CCntrItem::~CCntrItem()
{
	// TODO: �������� ��� �������
}

void CCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);

	// �� ����� �������������� �������� (���� �� �����, ���� ����� �� ��������� ������),
	//  ������������ ����������� OnChange �� ��������� ���������
	//  ����� �������� ��� ����������� ����������� ��� �����������.

	// TODO: �������� ������� ������������ ����������� ������ UpdateAllViews
	//  (� ����������, ����������� ��� ������ ����������)

	GetDocument()->UpdateAllViews(NULL);
		// �������� ��� ������������� ��� ��������
}

BOOL CCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	// �� ����� ��������� �� ����� CCntrItem::OnChangeItemPosition
	//  ���������� �������� ��� ��������� ��������� ����
	//  �� �����. ������ ��� ��������� � ������, ����� ������ ���������
	//  �� ������� �������� ����� �������, ��� ���������� ��� ��������, ���� � ������
	//  ��������� �������� �� �����.
	//
	// �� ��������� ���������� ������� �����, ������� ��������
	//  COleClientItem::SetItemRects ��� ����������� ��������
	//  � ����� ���������.

	if (!COleClientItem::OnChangeItemPosition(rectPos))
		return FALSE;

	// TODO: �������� ��������� ��� �������������� ��� ��������

	return TRUE;
}

BOOL CCntrItem::OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow)
{
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, pFrameWnd);

	if (pMainFrame != NULL)
	{
		ASSERT_VALID(pMainFrame);
		return pMainFrame->OnShowPanes(bShow);
	}

	return FALSE;
}

void CCntrItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);

	// �� ����� ��������� �� ����� CCntrItem::OnGetItemPosition
	//  ����� ������ ��� ����������� ������������ ����� ��������. ������ ���� 
	//  ������������� ���������� ������� ��������� �������� �� ��������� � 
	//  �������������, ������������� ��� ���������. ����� �������� ��� �������������, �������� 
	//  CCntrItem::GetActiveView.

	// TODO: ���������� ���������� ������������� (� ��������) � rPosition

	CSize size;
	rPosition.SetRectEmpty();
	if (GetExtent(&size, m_nDrawAspect))
	{
		CMinView* pView = GetActiveView();
		ASSERT_VALID(pView);
		if (!pView)
			return;
		CDC *pDC = pView->GetDC();
		ASSERT(pDC);
		if (!pDC)
			return;
		pDC->HIMETRICtoLP(&size);
		rPosition.SetRect(10, 10, size.cx + 10, size.cy + 10);
	}
	else
		rPosition.SetRect(10, 10, 210, 210);
}

void CCntrItem::OnActivate()
{
    // ��������� ������ ���� ������� ��������� �� ����� ��� ����� �����
    CMinView* pView = GetActiveView();
    ASSERT_VALID(pView);
	if (!pView)
		return;
    COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
    if (pItem != NULL && pItem != this)
        pItem->Close();
    
    COleClientItem::OnActivate();
}

void CCntrItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

    DWORD dwMisc = 0;
    m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
    if (dwMisc & OLEMISC_INSIDEOUT)
        DoVerb(OLEIVERB_HIDE, NULL);
}

void CCntrItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	// �������� ������� ������� ����� ��� ������ ������ COleClientItem.
	// ��������� ��� ���� ������������� ��������� ��������� m_pDocument, ������������� ��
	//  CCntrItem::GetDocument, ������������� ������� �������
	//  ������� ����� Serialize.
	COleClientItem::Serialize(ar);

	// ��� �������� ��� ���������� ������, ����������� � CCntrItem
	if (ar.IsStoring())
	{
		// TODO: �������� ��� ����������
	}
	else
	{
		// TODO: �������� ��� ��������
	}
}

BOOL CCntrItem::CanActivate()
{
	// ����� �������������� �� �����, ����� ��� ������ ������������� �� �����,
	//  �� �������� � �� ��������������. ������� � ���� ������
	//  ��������� �� ����� ������� ���������.
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return FALSE;
	ASSERT_KINDOF(COleServerDoc, pDoc);
	if (!pDoc->IsKindOf(RUNTIME_CLASS(COleServerDoc)))
	{
		return FALSE;
	}
	if (pDoc->IsInPlaceActive())
		return FALSE;

	// � ��������� ������ ������������ ��������� �� ���������
	return COleClientItem::CanActivate();
}


// ����������� CCntrItem

#ifdef _DEBUG
void CCntrItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CCntrItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif

