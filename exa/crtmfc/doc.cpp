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

// doc.cpp : ���������� ������ CDoc
//

#include "stdafx.h"
// SHARED_HANDLERS ����� ���������� � ������������ �������� ��������� ���������� ������� ATL, �������
// � ������; ��������� ��������� ������������ ��� ��������� � ������ �������.
#ifndef SHARED_HANDLERS
#include "cm.app.h"
#endif

#include "doc.h"
#include "cntritem.h"
#include "srvritem.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDoc

IMPLEMENT_DYNCREATE(CDoc, COleServerDocEx)

BEGIN_MESSAGE_MAP(CDoc, COleServerDocEx)
	// �������� ���������� ���������� OLE
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &COleServerDocEx::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, &COleServerDocEx::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, &COleServerDocEx::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, &COleServerDocEx::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, &COleServerDocEx::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, &COleServerDocEx::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, &COleServerDocEx::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_FILE_SEND_MAIL, &CDoc::OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, &CDoc::OnUpdateFileSendMail)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDoc, COleServerDocEx)
END_DISPATCH_MAP()

// ����������. ��������� ��������� ��� IID_Icrtmfc ��� ��������� �������� typesafe
//  �� VBA. IID ������ ��������� � GUID, ������� ������ � 
//  disp-��������� � ����� .IDL.

// {1AABBAE6-0AB9-4CC2-A6B3-DD56F1C5A611}
static const IID IID_Icrtmfc =
{ 0x1AABBAE6, 0xAB9, 0x4CC2, { 0xA6, 0xB3, 0xDD, 0x56, 0xF1, 0xC5, 0xA6, 0x11 } };

BEGIN_INTERFACE_MAP(CDoc, COleServerDocEx)
	INTERFACE_PART(CDoc, IID_Icrtmfc, Dispatch)
END_INTERFACE_MAP()


// ��������/����������� CDoc

CDoc::CDoc()
{
	// ����������� ������������ ����� OLE
	EnableCompoundFile();

	// TODO: �������� ��� ��� ������������ ������ ������������

	EnableAutomation();

	AfxOleLockApp();
}

CDoc::~CDoc()
{
	AfxOleUnlockApp();
}

BOOL CDoc::OnNewDocument()
{
	if (!COleServerDocEx::OnNewDocument())
		return FALSE;

	// TODO: �������� ��� ��������� �������������
	// (��������� SDI ����� �������� ������������ ���� ��������)

	return TRUE;
}


// ���������� ������� CDoc

COleServerItem* CDoc::OnGetEmbeddedItem()
{
	// OnGetEmbeddedItem ���������� ������� ������ ��� ��������� COleServerItem,
	//  ������� ������ � ���� ����������. ���������� ��� �������������.

	CSrvrItem* pItem = new CSrvrItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}




// ������������ CDoc

void CDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �������� ��� ����������
	}
	else
	{
		// TODO: �������� ��� ��������
	}

	// ����� �������� ������ COleServerDocEx �������� ������������
	//  �������� COleClientItem ��������� ����������.
	COleServerDocEx::Serialize(ar);
}

#ifdef SHARED_HANDLERS

// ��������� ��� �������
void CDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �������� ���� ��� ��� ����������� ������ ���������
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// ��������� ������������ ������
void CDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ������� ���������� ������ �� ������ ���������. 
	// ����� ����������� ������ ����������� ������ � ������� ";"

	// ��������:  strSearchContent = _T("�����;�������������;����;������ ole;");
	SetSearchContent(strSearchContent);
}

void CDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// ����������� CDoc

#ifdef _DEBUG
void CDoc::AssertValid() const
{
	COleServerDocEx::AssertValid();
}

void CDoc::Dump(CDumpContext& dc) const
{
	COleServerDocEx::Dump(dc);
}
#endif //_DEBUG


// ������� CDoc
