#pragma once

#include "dh.tracing.h"
#include <winuser.h>

namespace CF
{
    class GUILeaks
    {
    public:
        GUILeaks();
        ~GUILeaks();

        void StartCount();
        void PrintCurrentState();

    private:
        static void Load(DWORD& gdi, DWORD& usr);

        DWORD gdiCount_;
        DWORD usrCount_;
    };

    inline GUILeaks::GUILeaks() 
        : gdiCount_(0)
        , usrCount_(0)
    {
        StartCount();
    }

    inline GUILeaks::~GUILeaks() 
    {
        PrintCurrentState();
    }

    inline void GUILeaks::Load(DWORD& gdi, DWORD& usr)
    {
        gdi = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
        usr = GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS);
    }

    inline void GUILeaks::StartCount()
    {
        Load(gdiCount_, usrCount_);
        DH::TCPrintf(DH::Category::Module(), _T("GuiLeaks: At beg - GDI %4d, USER %4d\n"), gdiCount_, usrCount_);
    }

    inline void GUILeaks::PrintCurrentState()
    {
        DWORD gdiCount = 0;
        DWORD usrCount = 0;
        Load(gdiCount, usrCount);

        DH::TCPrintf(DH::Category::Module(), _T("GuiLeaks: At end - GDI %4d, USER %4d\n"), gdiCount, usrCount);
        DH::TCPrintf(DH::Category::Module(), _T("GuiLeaks: Leaks  - GDI %4d, USER %4d\n"), gdiCount - gdiCount_, usrCount - usrCount_);
    }
}

