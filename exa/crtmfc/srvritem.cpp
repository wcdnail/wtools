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

// srvritem.cpp : ���������� ������ CSrvrItem
//

#include "stdafx.h"
#include "cm.app.h"

#include "doc.h"
#include "srvritem.h"
#include "cntritem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���������� CSrvrItem

IMPLEMENT_DYNAMIC(CSrvrItem, COleServerItem)

CSrvrItem::CSrvrItem(CDoc* pContainerDoc)
	: COleServerItem(pContainerDoc, TRUE)
{
	// TODO: �������� ��� ��� ������������ ������ ������������
	//  (��������, ���������� �������������� �������� ��� ������ ������ � ��������� ������ ��������)
}

CSrvrItem::~CSrvrItem()
{
	// TODO: �������� ��� �������
}

void CSrvrItem::Serialize(CArchive& ar)
{
	// CSrvrItem::Serialize ���������� ������� ������, ����
	//  ������� ���������� � ����� ������. ��� ����� ��������� �������������
	//  ����������� ��������� ������ OLE OnGetClipboardData. ��� ����������� ��������
	//  �� ��������� ������������� ������ ������������ � ������� Serialize
	//  ���������. � ������ ��������� ������ �������� ����������� �������������
	//  ��������� ����� ���������.

	if (!IsLinkedItem())
	{
		CDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (pDoc)
			pDoc->Serialize(ar);
	}
}

BOOL CSrvrItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
	// ����������� ����������, �������� ����� ��� ���, ������������ ������ ��������� ��������� �����������
	//  ��������. ��� ��������� ������ ���������, ��������
	//  DVASPECT_THUMBNAIL (����� ���������� OnDrawEx) ���
	//  ���������� OnGetExtent ������ ���� �������� �� ���������
	//  �������������� ���������.

	if (dwDrawAspect != DVASPECT_CONTENT)
		return COleServerItem::OnGetExtent(dwDrawAspect, rSize);

	// CSrvrItem::OnGetExtent ���������� ��� ��������� �������� �
	//  �������� HIMETRIC ������ ��������. � ���������� �� ���������
	//  ������ ������������ ���������� ������.

	// TODO: �������� ���� ��������� ������

	rSize = CSize(3000, 3000);   // 3000 x 3000 HIMETRIC ������

	return TRUE;
}

BOOL CSrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{
	if (!pDC)
		return FALSE;

	// ������� ���� ���, ���� �� ����������� rSize
	UNREFERENCED_PARAMETER(rSize);

	// TODO: ������� ����� ����������� � ��������
	//  (�������� ������ ��������� � ��������, ������������ �� OnGetExtent)
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(0,0);
	pDC->SetWindowExt(3000, 3000);

	// TODO: �������� ��� ���������. ����� ����, ��������� �������� HIMETRIC.
	//  ��� ������� ������������ � ��������� ���������� ��������� (pDC).

	// TODO: ����� ��������� ���������� ������� CCntrItem.

	// ��������� ��� �������� ������ ������� � ��������� ���������.

	// TODO: ������� ���� ��� ����� ���������� ������ ���� ���������

	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return FALSE;

	POSITION pos = pDoc->GetStartPosition();
	CCntrItem* pItem = DYNAMIC_DOWNCAST(CCntrItem, pDoc->GetNextClientItem(pos));
	if (pItem != NULL)
		pItem->Draw(pDC, CRect(10, 10, 1010, 1010));
	return TRUE;
}


// ����������� CSrvrItem

#ifdef _DEBUG
void CSrvrItem::AssertValid() const
{
	COleServerItem::AssertValid();
}

void CSrvrItem::Dump(CDumpContext& dc) const
{
	COleServerItem::Dump(dc);
}
#endif

