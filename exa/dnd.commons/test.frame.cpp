#include "stdafx.h"
#include "test.frame.h"
#include "dnd.target.h"
#include <string.utils.error.code.h>
#include <dh.tracing.h>

void ShowErrorBox(HRESULT hr, PCWSTR caption, HWND parent /*= NULL*/)
{
    ::MessageBoxW(parent, Str::ErrorCode<wchar_t>::SystemMessage(hr), caption, MB_ICONSTOP);
}

namespace Test
{
    Frame::Frame()
        : Super()
        , DropTarget()
    {}

    Frame::~Frame()
    {}

    void Frame::OnActivate(UINT state, BOOL minimized, CWindow other)
    {}

    int Frame::OnCreate(LPCREATESTRUCT)
    {
        HRESULT hr = DropTarget.Register(m_hWnd);
        if (FAILED(hr))
        {
            ::ShowErrorBox(hr, L"RegisterDragDrop");
            return -1;
        }

        DlgResize_Init(false, false);
        return 0;
    }

    void Frame::OnDestroy()
    {
        ::PostQuitMessage(0);
        SetMsgHandled(FALSE);
    }

    BOOL Frame::OnEraseBkgnd(CDCHandle dc)
    {
        CRect rc;
        GetClientRect(rc);
        dc.FillSolidRect(rc, ::GetSysColor(COLOR_MENU));
        return 1;
    }
}
