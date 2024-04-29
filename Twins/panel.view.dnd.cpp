#include "stdafx.h"
#include "panel.view.dnd.h"
#include "panel.view.h"
#include "dnd.droptarget.h"
#include "dnd.dropsource.h"
#include "dnd.dataobject.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>
#include "res/resource.h"

namespace Twins
{
    DragnDropHelper::DragnDropHelper(PanelView& owner)
        : Owner(owner)
        , ClickTick(InvalidTick)
        , DragDetected(false)
        , Cursor()
        , Selection()
        , DTarget()
    {}

    DragnDropHelper::CursorSet::CursorSet()
        : DropMultiple(::LoadCursor(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDC_DROP_MULTIPLE)))
        , DropSingle(::LoadCursor(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDC_DROP_SINGLE)))
        , DropNo(::LoadCursor(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDC_DROP_NO)))
        , Current(NULL)
    {}

    DragnDropHelper::~DragnDropHelper()
    {}

    HWND DragnDropHelper::GetHwnd() const { return Owner; }

    int DragnDropHelper::OnCreate(LPCREATESTRUCT)
    {
        CComPtr<Dnd::DropTarget> dtarget = new Dnd::DropTarget(*this);

        HRESULT hr = ::RegisterDragDrop(Owner, dtarget);

        DH::ThreadPrintf(L"DRAGDROP: registering - 0x%x `%s`\n", hr, Str::ErrorCode<wchar_t>::SystemMessage(hr));

        if (SUCCEEDED(hr))
        {
            DTarget = dtarget;

            FORMATETC ftetc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
            DTarget->AddSuportedFormat(ftetc); 
        }

        SetMsgHandled(FALSE);
        return 0;
    }

    void DragnDropHelper::OnDestroy()
    {
        ::RevokeDragDrop(Owner);
        SetMsgHandled(FALSE);
    }

    void DragnDropHelper::OnLButtonDown(UINT flags, CPoint point) 
    {
        ClickTick = ::GetTickCount();
        Owner.SetCapture();
        SetMsgHandled(FALSE);
    }

    void DragnDropHelper::OnMouseMove(UINT flags, CPoint point) 
    {
        if (InvalidTick != ClickTick)
        {
            if (!DragDetected && ((::GetTickCount() - ClickTick) > 250))
            {
                DragDetected = true;
                OnStart(point);
            }
        }

        if (DragDetected)
            OnDrag(point);

        SetMsgHandled(FALSE);
    }

    void DragnDropHelper::OnLButtonUp(UINT flags, CPoint point) 
    {
        if (DragDetected)
        {
            DragDetected = false;

            STGMEDIUM dmy = {0};
            OnStop(point, dmy, NULL, NULL);
        }

        ::ReleaseCapture();
        ClickTick = InvalidTick;
        SetMsgHandled(FALSE);
    }

    void DragnDropHelper::OnStart(CPoint const& pt)
    {
        //Fl::List selection;
        //if (!Owner.GetSelection(selection))
        //    selection.PushBack(Owner.GetHot());
        //
        //Selection.Swap(selection);

        DH::ThreadPrintf(L"DRAGDROP: beg %s\n", IsMultipleSelection() ? L"multiple" : L"single");

        //Cursor.Current = IsMultipleSelection() ? Cursor.DropMultiple : Cursor.DropSingle;
        //::SetCursor(Cursor.DropNo);

        BeginDrag(Selection, pt);
    }

    void DragnDropHelper::OnStop(CPoint const& pt, STGMEDIUM& medium, FORMATETC* format, DWORD* effect)
    {
        DH::ThreadPrintf(L"DRAGDROP: end \n");
        //::SetCursor(Owner.GetDefaultCursor());
    }

    void DragnDropHelper::OnDrag(CPoint const& pt)
    {
// ##TODO: determine target point"))
        //::SetCursor(rc.PtInRect(screen) ? Cursor.DropNo : Cursor.Current);
    }

    bool DragnDropHelper::IsMyPoint(POINTL const& pt)
    {
        CRect rc;
        Owner.GetWindowRect(rc);

        CPoint screen(pt.x, pt.y);
        return TRUE == rc.PtInRect(screen);
    }

    HGLOBAL DragnDropHelper::PrepareHDROP(Fl::List const& files, CPoint const& pt)
    {
        size_t bs = sizeof(DROPFILES);

        for (Fl::List::const_iterator it = files.Begin(); it != files.End(); ++it)
            bs += sizeof(wchar_t) * (it->GetPath().length() + 1);

        bs += sizeof(wchar_t) * 2;

        HGLOBAL hgl = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bs);
        if ( NULL == hgl )
            return NULL;

        BYTE* buffer = (BYTE*)::GlobalLock(hgl);
        if (NULL == buffer)
        {
            ::GlobalFree(hgl);
            return NULL;
        }

        DROPFILES* drop = (DROPFILES*)buffer;
        drop->pFiles = sizeof(DROPFILES);
        drop->pt = pt;
        drop->fWide = 1;
        PWSTR filenames = (PWSTR)(buffer + sizeof(DROPFILES));

        for (Fl::List::const_iterator it = files.Begin(); it != files.End(); ++it)
        {
            size_t len = it->GetPath().length() + 1;
            ::wcscpy_s(filenames, len, it->GetPath().c_str());
            filenames += len;
        }

        ::GlobalUnlock(hgl);
        return hgl;
    }

    /*
    static HRESULT InitializeHelper(IDragSourceHelper* pDragSourceHelper
                                  , HBITMAP hBitmap
                                  , POINT& pt
                                  ,	RECT& rc
                                  , IDataObject* pDataObject
                                  , COLORREF crColorKey=GetSysColor(COLOR_WINDOW)
                                  )
    {
        if(pDragSourceHelper == NULL)
            return E_FAIL;

        SHDRAGIMAGE di;

        BITMAP bm;
        GetObject(hBitmap, sizeof(bm), &bm);
        di.sizeDragImage.cx = bm.bmWidth;
        di.sizeDragImage.cy = bm.bmHeight;
        di.hbmpDragImage = hBitmap;
        di.crColorKey = crColorKey; 
        di.ptOffset.x = pt.x - rc.left;
        di.ptOffset.y = pt.y - rc.top;

        return pDragSourceHelper->InitializeFromBitmap(&di, pDataObject);
    }
    */

    HRESULT DragnDropHelper::BeginDrag(Fl::List const& files, CPoint const& pt) const
    {
        CComPtr<Dnd::DropSource> source(new Dnd::DropSource());
        CComPtr<Dnd::DataObject> object(new Dnd::DataObject(source));

        HGLOBAL buffer = PrepareHDROP(files, pt);
        if (!buffer)
            return E_INVALIDARG;

        STGMEDIUM medium = {0};
        medium.tymed = TYMED_HGLOBAL;
        medium.hGlobal = buffer;

        FORMATETC format = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
        HRESULT hr = object->SetData(&format, &medium, TRUE);
        DH::ThreadPrintf(_T("DragDrop: Data 0x%x\n"), hr);
        if (SUCCEEDED(hr))
        {
#if 0
            CComPtr<IDragSourceHelper> helper;
            if (SUCCEEDED(helper.CoCreateInstance(CLSID_DragDropHelper)))
            {
// ##TODO: Init helper"))
                
                //CPoint pn = pt;
                //Owner.ClientToScreen(&pn);
                //hr = helper->InitializeFromWindow(Owner, &pn, object);
                //hr = InitializeHelper(helper, NULL, pt, )
                //DH::ThreadPrintf(_T("DragDrop: Help 0x%x\n"), hr);
            }
#endif

            DWORD dwEffect = 0;
            hr = ::DoDragDrop(object, source, DROPEFFECT_COPY, &dwEffect);
            DH::ThreadPrintf(_T("DragDrop: DoDD 0x%x\n"), hr);
        }
        else
        {
            ::GlobalFree(buffer);
        }

        return hr;
    } 

    //UINT DragnDropHelper::WM_DI_GETDRAGIMAGE = ::RegisterWindowMessage(DI_GETDRAGIMAGE);

    //LRESULT DragnDropHelper::OnGetDragImage(UINT, WPARAM wParam, LPARAM lParam)
    //{
    //    CClientDC sourceDc(::GetDesktopWindow());

    //    CBitmap bmp;
    //    bmp.CreateCompatibleBitmap(sourceDc, 64, 64);

    //    CDC dc;
    //    dc.CreateCompatibleDC(sourceDc);
    //    HBITMAP prev = dc.SelectBitmap(bmp);

    //    LPSHDRAGIMAGE di = (LPSHDRAGIMAGE)lParam;
    //    di->sizeDragImage.cx = 64;
    //    di->sizeDragImage.cy = 64;
    //    di->ptOffset.x = 0;
    //    di->ptOffset.y = 0;
    //    di->hbmpDragImage = bmp.Detach();
    //    di->crColorKey = ::GetSysColor(COLOR_WINDOW);

    //    return 0;
    //}
}
