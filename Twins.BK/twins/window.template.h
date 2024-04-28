#pragma once

#include <atlwin.h>

namespace Twins
{
    class BasicViewer: CWindowImpl<BasicViewer>
    {
    public:
        typedef CWindowImpl<BasicViewer> Super;

        ~BasicViewer();
        BasicViewer();

        void Show(CRect& rc, HWND parent = NULL);

    private:
        friend Super;

        HWND Editor;

        BEGIN_MSG_MAP_EX(BasicViewer)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()

        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
    };
}
