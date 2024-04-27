#include "stdafx.h"
#include "basic.viewer.h"

namespace Twins
{
    BasicViewer::~BasicViewer()
    {}

    BasicViewer::BasicViewer()
        : Super()
        , Editor(NULL)
    {
    }

    void BasicViewer::Show(CRect& rc, HWND parent /*= NULL*/)
    {
        if (!m_hWnd)
            Super::Create(parent, rc, NULL, WS_OVERLAPPEDWINDOW, 0);

        ShowWindow(SW_SHOW);
    }

    int BasicViewer::OnCreate(LPCREATESTRUCT cs)
    {
        return 0;
    }

    void BasicViewer::OnDestroy()
    {
        SetMsgHandled(FALSE);

#ifdef _DEBUG
        ::PostQuitMessage(0);
#endif
    }
}
