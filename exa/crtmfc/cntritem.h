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

// cntritem.h : ��������� ������ CCntrItem
//

#pragma once

class CDoc;
class CMinView;

class CCntrItem : public COleClientItem
{
	DECLARE_SERIAL(CCntrItem)

// ������������
public:
	CCntrItem(CDoc* pContainer = NULL);
		// ����������. pContainer ����� ����� �������� NULL ��� ���������� IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE ������� ������� � ������ ������������ �
		//  ������� �����������. ������ �������� OLE ��������� �
		//  ����������� �� ��������, �� ������� NULL

// ��������
public:
	CDoc* GetDocument()
		{ return reinterpret_cast<CDoc*>(COleClientItem::GetDocument()); }
	CMinView* GetActiveView()
		{ return reinterpret_cast<CMinView*>(COleClientItem::GetActiveView()); }

public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();

protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual BOOL OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow);
	virtual BOOL CanActivate();

// ����������
public:
	~CCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

