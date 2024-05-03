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

// view.cpp : ���������� ������ CMinView
//

#include "stdafx.h"
// SHARED_HANDLERS ����� ���������� � ������������ �������� ��������� ���������� ������� ATL, �������
// � ������; ��������� ��������� ������������ ��� ��������� � ������ �������.
#ifndef SHARED_HANDLERS
#include "cm.app.h"
#endif

#include "doc.h"
#include "cntritem.h"
#include "resource.h"
#include "view.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMinView

IMPLEMENT_DYNCREATE(CMinView, CView)

BEGIN_MESSAGE_MAP(CMinView, CView)
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_OLE_INSERT_NEW, &CMinView::OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, &CMinView::OnCancelEditCntr)
	ON_COMMAND(ID_FILE_PRINT, &CMinView::OnFilePrint)
	ON_COMMAND(ID_CANCEL_EDIT_SRVR, &CMinView::OnCancelEditSrvr)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMinView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// ��������/����������� CMinView

CMinView::CMinView()
{
	EnableActiveAccessibility();
	m_pSelection = NULL;
	// TODO: �������� ��� ��������

}

CMinView::~CMinView()
{
}

BOOL CMinView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �������� ����� Window ��� ����� ����������� ���������
	//  CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// ��������� CMinView

void CMinView::OnDraw(CDC* pDC)
{
	if (!pDC)
		return;

	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �������� ����� ��� ��������� ��� ����������� ������
	// TODO: ����� ��������� ��� �������� OLE � ���������

	// ��������� ���������� �������� � ������������ �����. ���� ��� ������ ����
	//  ������, ��������� ���������� ��� ���������. ��� ���������
	//  ����� ������������� ��������������, ������������� CCntrItem,
	//  ��� �������������� ����������� �������������� �� �����.

	// TODO: ������� ���� ��� ����� ���������� ������ ���� ���������.
	if (m_pSelection != NULL)
	{
		CSize size;
		CRect rect(10, 10, 210, 210);
		
		if (m_pSelection->GetExtent(&size, m_pSelection->m_nDrawAspect))
		{
			pDC->HIMETRICtoLP(&size);
			rect.right = size.cx + 10;
			rect.bottom = size.cy + 10;
		}
		m_pSelection->Draw(pDC, rect);
	}
}

void CMinView::OnInitialUpdate()
{
	CView::OnInitialUpdate();


	// TODO: ������� ���� ���, ����� ��������� ��� ������ ������ ����� �������
	m_pSelection = NULL;    // ������������� ���������

}


// ������ CMinView


void CMinView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMinView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// ���������� �� ���������
	return DoPreparePrinting(pInfo);
}

void CMinView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �������� �������������� ������������� ����� �������
}

void CMinView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �������� ������� ����� ������
}

void CMinView::OnDestroy()
{
	// ������������� ������� ��� ��������; ��� �����
	// � ������ ������������� ������������� �����������
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
   CView::OnDestroy();
}



// ������� � ��������� ������� OLE

BOOL CMinView::IsSelected(const CObject* pDocItem) const
{
	// ���������� ���� �����������, ���� ���������� �������� �������
	//  ������ �� �������� CCntrItem. ����� ��������� ����������
	//  ����������� ������ ������� �������� ����� ����������

	// TODO: ���������� ��� �������, ������� ��������� ��������� ������� ������� OLE

	return pDocItem == m_pSelection;
}

void CMinView::OnInsertObject()
{
	// �������� ����������� ���������� ���� "������� �������" ��� ��������� ����������
	//  ��� ������ ������� CCntrItem
	COleInsertDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	BeginWaitCursor();

	CCntrItem* pItem = NULL;
	TRY
	{
		// ������� ����� �������, ��������� � ���� ����������
		CDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pItem = new CCntrItem(pDoc);
		ASSERT_VALID(pItem);

		// ��������������� ������� �� ������ ����������� ����
		if (!dlg.CreateItem(pItem))
			AfxThrowMemoryException();  // ����� ���������� ��������
		ASSERT_VALID(pItem);
		
        if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);

		ASSERT_VALID(pItem);
		// ��������� ������ ���������� ������������ ������������, ��� ������ ������� �����������
		//  ��������� � ��������� �������

		// TODO: ��������� ���������� ���������, ��� ��� ��������� � ����������
		m_pSelection = pItem;   // ��������� ���������� ������������ ��������
		pDoc->UpdateAllViews(NULL);
	}
	CATCH(CException, e)
	{
		if (pItem != NULL)
		{
			ASSERT_VALID(pItem);
			pItem->Delete();
		}
		AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH

	EndWaitCursor();
}

// ��������� ���������� ������ ������������� ����������� ������������
//  ��������� ������������ ��� ������ ������ �������������� �� �����. �����
//  ��������� (�� ������) �������� �����������
void CMinView::OnCancelEditCntr()
{
	// �������� � ���� ������������� ��� �������� ������� �������������� �� �����
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

// ��� ���������� ��������� ����������� ��������� OnSetFocus � OnSize,
//  ���� ������ ������������� �� �����
void CMinView::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// ���������� ���������� ����� �� ���� ������, ���� �� ��������� � ��� �� �������������
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();   // �� ��������� ������� �����
			return;
		}
	}

	CView::OnSetFocus(pOldWnd);
}

void CMinView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
		pActiveItem->SetItemRects();
}

void CMinView::OnFilePrint()
{
	//�� ��������� ������ ��������� ��������� �������������
	//� ������� IOleCommandTarget. ���� ����� ��������� �� ���������,
	//������� ����� COleDocObjectItem::DoDefaultPrinting.
	//���� ����� ����� ��������� �� �����-���� �������, �� ��������� ����������
	//docobject � ������� ���������� IPrint.
	CPrintInfo printInfo;
	ASSERT(printInfo.m_pPD != NULL); 
	if (S_OK == COleDocObjectItem::DoDefaultPrinting(this, &printInfo))
		return;
	
	CView::OnFilePrint();

}


void CMinView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMinView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	_App.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// ��������� ������� OLE

// ��������� ���������� ������ ������������� ����������� ������������
//  ��������� ������������ ��� ������ ������ �������������� �� �����. �����
//  ������ (�� ���������) �������� �����������
void CMinView::OnCancelEditSrvr()
{
	GetDocument()->OnDeactivateUI(FALSE);
}


// ����������� CMinView

#ifdef _DEBUG
void CMinView::AssertValid() const
{
	CView::AssertValid();
}

void CMinView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDoc* CMinView::GetDocument() const // �������� ������������ ������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
	return (CDoc*)m_pDocument;
}
#endif //_DEBUG


// ����������� ��������� CMinView
