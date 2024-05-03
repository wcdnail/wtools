#pragma once

#include <atlcom.h>
#include <oleidl.h>
#include <shobjidl.h>

namespace Dnd
{
    class Target: public IDropTarget
    {
    public:
        Target();
        virtual ~Target();

        HRESULT Register(HWND client);

    private:
        CComPtr<IDropTargetHelper> Helper;
        HWND Client;

        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void** ppvObject) { return E_NOTIMPL; }
        virtual ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }
        virtual ULONG STDMETHODCALLTYPE Release(void) { return 1; }

        virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
        virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
        virtual HRESULT STDMETHODCALLTYPE DragLeave(void);
        virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
    };
}
