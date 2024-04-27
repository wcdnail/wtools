#ifndef _CF_WTL_LISTVIEW_HELPERS_HXX__
#define _CF_WTL_LISTVIEW_HELPERS_HXX__

#include <boost/noncopyable.hpp>
#include <atlctrls.h>

namespace Cf
{
    namespace ListView
    {
        template <typename T>
        struct Scoped_RedrawLock: boost::noncopyable
        {
            Scoped_RedrawLock(T const& cliref) 
                : client(const_cast<T*>(&cliref))
            {
                client->SetRedraw(FALSE);
            }

            ~Scoped_RedrawLock() 
            {
                client->SetRedraw(TRUE);
            }

        private:
            T* client;
        };
    }

    inline void ClearHeaderItems(WTL::CHeaderCtrl& header)
    {
        int count = header.GetItemCount();

        for (int i=0; i<count; i++)
            header.DeleteItem(i);
    }
}

#endif /* _CF_WTL_LISTVIEW_HELPERS_HXX__ */
