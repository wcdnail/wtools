#include "stdafx.h"
#include "dnd.test.h"
#include "dnd.droptarget.h"
#include "dnd.dropsource.h"
#include "dnd.dataobject.h"
#include <dh.tracing.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <atlcomcli.h>
#include <atlcom.h>

namespace Simple
{
    struct DropTarget: public Dnd::DropTarget
    {
        DropTarget(HWND window)
            : Dnd::DropTarget(window)
        {}

        virtual bool OnDrop(FORMATETC* format, STGMEDIUM& medium, DWORD* effect) 
        {
            return true;
        }
    };

    static CComPtr<DropTarget> gDropTarget;

    void InitDragdrop(HWND window)
    {
        gDropTarget = new DropTarget(window);

        ::RegisterDragDrop(window, gDropTarget);

        FORMATETC ftetc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
        gDropTarget->AddSuportedFormat(ftetc); 
    }

    void DestroyDragdrop(HWND window)
    {
        ::RevokeDragDrop(window);
        gDropTarget = NULL;
    }

    HGLOBAL PrepareDragGlobal(FileList const& files, POINT const& pt);

    HRESULT BeginDrag(HWND window, POINT& pt, FileList const& files)
    {
        CComPtr<Dnd::DropSource> source(new Dnd::DropSource());
        CComPtr<Dnd::DataObject> object(new Dnd::DataObject(source));

        HGLOBAL buffer = PrepareDragGlobal(files, pt);
        if (!buffer)
            return E_INVALIDARG;

        STGMEDIUM medium = {0};
        medium.tymed = TYMED_HGLOBAL;
        medium.hGlobal = buffer;

        FORMATETC format = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
        HRESULT hr = object->SetData(&format, &medium, TRUE);
        Dh::ThreadPrintf(_T("DragDrop: SetData - 0x%x\n"), hr);
        if (SUCCEEDED(hr))
        {
            CComPtr<IDragSourceHelper> helper;
            if (SUCCEEDED(helper.CoCreateInstance(CLSID_DragDropHelper)))
            {
                hr = helper->InitializeFromWindow(window, &pt, object);
                Dh::ThreadPrintf(_T("DragDrop: Helper - 0x%x\n"), hr);
            }

            DWORD dwEffect = 0;
            hr = ::DoDragDrop(object, source, DROPEFFECT_COPY, &dwEffect);
            Dh::ThreadPrintf(_T("DragDrop: Do - 0x%x\n"), hr);
        }
        else
        {
            ::GlobalFree(buffer);
        }

        return hr;
    } 

    static HGLOBAL PrepareDragGlobal(FileList const& files, POINT const& pt)
    {
        size_t bs = sizeof(DROPFILES);

        for (FileList::const_iterator it = files.begin(); it != files.end(); ++it)
            bs += sizeof(wchar_t) * (it->length() + 1);

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

        for (FileList::const_iterator it = files.begin(); it != files.end(); ++it)
        {
            size_t len = it->length() + 1;
            ::wcscpy_s(filenames, len, it->c_str());
            filenames += len;
        }

        ::GlobalUnlock(hgl);
        return hgl;
    }

    void GetFileList(FileList& files)
    {
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.dataobject.cpp");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.dataobject.h");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.dragsource.helper.cpp");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.dragsource.helper.h");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.dropsource.cpp");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.dropsource.h");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.droptarget.cpp");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.droptarget.h");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.enumformat.cpp");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.enumformat.h");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.test.cpp");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\dnd.test.h");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\MAIN.cpp");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\res");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\stdafx.cpp");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\stdafx.h");
        files.push_back(L"c:\\++\\projroot\\unit.tests\\targetver.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\unit.tests.10.vcxproj");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\unit.tests.10.vcxproj.filters");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\unit.tests.10.vcxproj.user");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\win32.host.app.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\win32.host.app.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\win32.host.window.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\win32.host.window.h");

        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\change.drive.notify.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\debug.consts.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\debug.consts.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\dialogz.directory.watch.test.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\dialogz.directory.watch.test.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\directory.watch.jnotify.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\directory.watch.jnotify.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\directory.watch.msdn.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\large.progress.test.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\memory.stuff.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\memory.stuff.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\native.window.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\native.window.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\panel.state.test.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\unit.tests.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\unit.tests.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\ut.wtl.application.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\ut.wtl.control.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\ut.wtl.control.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\ut.wtl.window.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\obsolete\\ut.wtl.window.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\res\\lime48.ico");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\res\\resource.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\res\\tb.bmp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\res\\unit.tests.aps");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\res\\unit.tests.exe.manifest");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\res\\unit.tests.rc");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\DragDropImpl.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\DragDropImpl.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\controls.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\controls.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\DragDrop.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\DragDrop.dsp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\DragDrop.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\DragDrop.rc");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\DragDropImpl.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\DragDropImpl.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\editdroptarget.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\listviewdroptarget.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\maindlg.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\readme.txt");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\res");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\resource.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\staticdroptarget.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\stdafx.cpp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\stdafx.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\treedroptarget.h");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\res\\anim.bmp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\res\\bitmap1.bmp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\res\\bmp00001.bmp");
        //files.push_back(L"c:\\++\\projroot\\unit.tests\\temp\\dragdrop\\res\\DragDrop.ico");
    }
}
