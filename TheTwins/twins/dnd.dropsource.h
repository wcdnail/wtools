#ifndef dnd_dropsource_h__
#define dnd_dropsource_h__

#include <com.iunknown.impl.h>
#include <oleidl.h>

namespace Dnd
{
    class DropSource: public IUnknownImp<IDropSource>
    {
    public:
        DropSource();

        virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL escape, DWORD state);
        virtual HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD effect);

    protected:
        virtual ~DropSource();
        
    public:
        bool IsDropped;
    };
}

#endif // dnd_dropsource_h__
