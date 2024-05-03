// Этот исходный код MFC Samples демонстрирует функционирование пользовательского интерфейса Fluent на основе MFC в Microsoft Office
// ("Fluent UI") и предоставляется исключительно как справочный материал в качестве дополнения к
// справочнику по пакету Microsoft Foundation Classes и связанной электронной документации,
// включенной в программное обеспечение библиотеки MFC C++. 
// Условия лицензионного соглашения на копирование, использование или распространение Fluent UI доступны отдельно. 
// Для получения дополнительных сведений о нашей лицензионной программе Fluent UI посетите веб-узел
// http://msdn.microsoft.com/officeui.
//
// (C) Корпорация Майкрософт (Microsoft Corp.)
// Все права защищены.

// cntritem.cpp : реализация класса CCntrItem
//

#include "stdafx.h"
#include "cm.app.h"

#include "doc.h"
#include "view.h"
#include "cntritem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// реализация CCntrItem

IMPLEMENT_SERIAL(CCntrItem, COleClientItem, 0)

CCntrItem::CCntrItem(CDoc* pContainer)
	: COleClientItem(pContainer)
{
	// TODO: добавьте код для одноразового вызова конструктора
}

CCntrItem::~CCntrItem()
{
	// TODO: добавьте код очистки
}

void CCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);

	// Во время редактирования элемента (либо по месту, либо когда он полностью открыт),
	//  отправляются уведомления OnChange об изменении состояния
	//  этого элемента или визуального изображения его содержимого.

	// TODO: сделайте элемент недопустимым посредством вызова UpdateAllViews
	//  (с указаниями, подходящими для вашего приложения)

	GetDocument()->UpdateAllViews(NULL);
		// обновите ВСЕ представления без указаний
}

BOOL CCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	// Во время активации по месту CCntrItem::OnChangeItemPosition
	//  вызывается сервером для изменения положения окна
	//  по месту. Обычно это возникает в случае, когда данные документа
	//  на сервере изменены таким образом, что изменяется его величина, либо в случае
	//  изменения размеров по месту.
	//
	// По умолчанию вызывается базовый класс, который вызывает
	//  COleClientItem::SetItemRects для перемещения элемента
	//  в новое положение.

	if (!COleClientItem::OnChangeItemPosition(rectPos))
		return FALSE;

	// TODO: обновите имеющийся кэш прямоугольника или величины

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

	// Во время активации по месту CCntrItem::OnGetItemPosition
	//  будет вызван для определения расположения этого элемента. Обычно этот 
	//  прямоугольник определяет текущее положение элемента по отношению к 
	//  представлению, используемому для активации. Чтобы получить это представление, вызовите 
	//  CCntrItem::GetActiveView.

	// TODO: возвратите правильный прямоугольник (в пикселах) в rPosition

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
    // Разрешить только один элемент активации по месту для одной рамки
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

	// Вызовите сначала базовый класс для чтения данных COleClientItem.
	// Поскольку при этом настраивается положение указателя m_pDocument, возвращенного из
	//  CCntrItem::GetDocument, рекомендуется сначала вызвать
	//  базовый класс Serialize.
	COleClientItem::Serialize(ar);

	// для хранения или извлечения данных, относящихся к CCntrItem
	if (ar.IsStoring())
	{
		// TODO: добавьте код сохранения
	}
	else
	{
		// TODO: добавьте код загрузки
	}
}

BOOL CCntrItem::CanActivate()
{
	// Режим редактирования по месту, когда сам сервер редактируется по месту,
	//  не работает и не поддерживается. Поэтому в этом случае
	//  активацию по месту следует отключить.
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

	// в противном случае используется поведение по умолчанию
	return COleClientItem::CanActivate();
}


// диагностика CCntrItem

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

