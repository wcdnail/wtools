#ifndef dnd_droptarget_h__
#define dnd_droptarget_h__

#include <com.iunknown.impl.h>
#include <oleidl.h>
#include <shobjidl.h>
#include <atlcomcli.h>

namespace Twins { class DragnDropHelper; }

namespace Dnd
{
    class DropTarget: public IUnknownImp<IDropTarget>
    {
    public:
        DropTarget(Twins::DragnDropHelper& owner);
        
        void AddSuportedFormat(FORMATETC& ftetc);

        virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* object, DWORD state, POINTL pt, DWORD* effect);
        virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD state, POINTL pt, DWORD* effect);
        virtual HRESULT STDMETHODCALLTYPE DragLeave();    
        virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject* object, DWORD state, POINTL pt, DWORD* effect);

    protected:
        virtual ~DropTarget();

    private:
        Twins::DragnDropHelper& Owner;
        bool AllowDrop;
        CComPtr<IDropTargetHelper> Helper;
        CSimpleArray<FORMATETC> Formats;
        FORMATETC* SupportedFormat;

        bool QueryDrop(DWORD state, POINTL const& pt, LPDWORD effect);
    };
}

#endif // dnd_droptarget_h__
