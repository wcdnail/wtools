#pragma once

#include <dh.tracing.h>
#include <winuser.h>

namespace CF
{
    class GUILeaks
    {
    public:
        DELETE_COPY_MOVE_OF(GUILeaks);

        GUILeaks();
        ~GUILeaks();

        void StartCount();
        void PrintCurrentState() const;

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
        DH::TPrintf(TL_Module, L"GuiLeaks: At start - GDI %d, USER %d\n", gdiCount_, usrCount_);
    }

    inline void GUILeaks::PrintCurrentState() const
    {
        DWORD gdiCount = 0;
        DWORD usrCount = 0;
        Load(gdiCount, usrCount);

        DH::TPrintf(TL_Module, L"GuiLeaks: At end - GDI %4d, USER %4d\n", gdiCount, usrCount);
        DH::TPrintf(TL_Module, L"GuiLeaks: Leaks  - GDI %4d, USER %4d\n", gdiCount - gdiCount_, usrCount - usrCount_);
    }
}

