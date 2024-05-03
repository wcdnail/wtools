#ifndef dnd_dragsource_helper_h__
#define dnd_dragsource_helper_h__

#if 0
#include <boost/noncopyable.hpp>
#include <shobjidl.h>

namespace Dnd
{
    class DragSourceHelper: boost::noncopyable
    {
    public:
        DragSourceHelper();
        virtual ~DragSourceHelper();

        // IDragSourceHelper
        HRESULT InitializeFromBitmap(HBITMAP hBitmap 
                                   , POINT& pt  /* cursor position in client coords of the window */
                                   , RECT& rc   /* selected item's bounding rect */
                                   , IDataObject* pDataObject
                                   , COLORREF crColorKey = GetSysColor(COLOR_WINDOW));
                                   /* color of the window used for transparent effect. */

        HRESULT InitializeFromWindow(HWND hwnd, POINT& pt,IDataObject* pDataObject);

    private:
        IDragSourceHelper* pDragSourceHelper;
    };
}
#endif

#endif // dnd_dragsource_helper_h__
