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

// srvritem.cpp : реализация класса CSrvrItem
//

#include "stdafx.h"
#include "cm.app.h"

#include "doc.h"
#include "srvritem.h"
#include "cntritem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// реализация CSrvrItem

IMPLEMENT_DYNAMIC(CSrvrItem, COleServerItem)

CSrvrItem::CSrvrItem(CDoc* pContainerDoc)
	: COleServerItem(pContainerDoc, TRUE)
{
	// TODO: добавьте код для одноразового вызова конструктора
	//  (например, добавление дополнительных форматов для буфера обмена в источнике данных элемента)
}

CSrvrItem::~CSrvrItem()
{
	// TODO: добавьте код очистки
}

void CSrvrItem::Serialize(CArchive& ar)
{
	// CSrvrItem::Serialize вызывается рабочей средой, если
	//  элемент копируется в буфер обмена. Это может произойти автоматически
	//  посредством обратного вызова OLE OnGetClipboardData. Для внедренного элемента
	//  по умолчанию рекомендуется просто делегировать в функцию Serialize
	//  документа. В случае поддержки ссылок возможно потребуется сериализовать
	//  отдельную часть документа.

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
	// Большинство приложений, например такие как это, обрабатывают только отрисовку элементов содержимого
	//  элемента. Для поддержки других элементов, например
	//  DVASPECT_THUMBNAIL (путем перезаписи OnDrawEx) эта
	//  реализация OnGetExtent должна быть изменена на обработку
	//  дополнительных элементов.

	if (dwDrawAspect != DVASPECT_CONTENT)
		return COleServerItem::OnGetExtent(dwDrawAspect, rSize);

	// CSrvrItem::OnGetExtent вызывается для получения величины в
	//  единицах HIMETRIC целого элемента. В реализации по умолчанию
	//  просто возвращается количество единиц.

	// TODO: замените этот случайный размер

	rSize = CSize(3000, 3000);   // 3000 x 3000 HIMETRIC единиц

	return TRUE;
}

BOOL CSrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{
	if (!pDC)
		return FALSE;

	// Удалите этот код, если вы используете rSize
	UNREFERENCED_PARAMETER(rSize);

	// TODO: задайте режим отображения и величину
	//  (Величина обычно совпадает с размером, возвращенным из OnGetExtent)
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(0,0);
	pDC->SetWindowExt(3000, 3000);

	// TODO: добавьте код отрисовки. Кроме того, заполните величину HIMETRIC.
	//  Все рисунки производятся в контексте устройства метафайла (pDC).

	// TODO: также нарисуйте внедренные объекты CCntrItem.

	// Следующий код нарисует первый элемент в случайном положении.

	// TODO: удалите этот код после завершения вашего кода рисования

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


// диагностика CSrvrItem

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

