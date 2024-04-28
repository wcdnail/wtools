#pragma once 

#if 0

#include <objbase.h>

namespace Initialize
{
    //
    // COM 
    // 
    struct COM
    {
        static const bool Multithreaded = true; 
        static const bool Singlethreaded = false;

        COM(bool mt = Multithreaded, DWORD method = COINIT_MULTITHREADED)
        {
            HRESULT hr;

            if (mt)
                hr = ::CoInitializeEx(NULL, method);
            else
                hr = ::CoInitialize(NULL);
                    
            if (FAILED(hr))
                throw AFX_FAULT("COM initialization failed: ")
                    (Afx::SystemMessage<char>(hr))
                    ("\n");
        }

        ~COM() /* throw() */
        {
            ::CoUninitialize();
        }
    };

    //
    // ATL/WTL 
    // 
    struct GUI
    {
        GUI(HINSTANCE base
            , DWORD commonControlsFlags = ICC_BAR_CLASSES
            , _ATL_OBJMAP_ENTRY* objMap = NULL
            , GUID const* libGUID = NULL
            ) /* throw(Afx::Fault) */
        {
            DefWindowProc(NULL, 0, 0, 0L);

            SetLastError(3);

            INITCOMMONCONTROLSEX iccx = { sizeof(iccx), commonControlsFlags };
            if (!::InitCommonControlsEx(&iccx))
                throw AFX_FAULT("AtlInitCommonControls failed\n");

            HRESULT hr = _Module.Init(objMap, base, libGUID);
            if (FAILED(hr))
                throw AFX_FAULT("CAppModule::Init(")
                    ("0x")(std::hex)(objMap)
                    (", 0x")(std::hex)(base)
                    (", 0x")(std::hex)(libGUID)
                    (") failed: ")
                    (Afx::SystemMessage<char>(hr))
                    ("\n");

            if (!AtlAxWinInit())
                throw AFX_FAULT("AtlAxWinInit failed: ")
                    (Afx::SystemMessage<char>(hr))
                    ("\n");

            _Module.AddMessageLoop(&loop_);
        }

        ~GUI() /* throw() */
        {
            _Module.RemoveMessageLoop();
            _Module.Term();
        }

        CMessageLoop& GetMessageLoop() { return loop_; }

    private:
        CMessageLoop loop_;
    };

    struct GDIPlus
    {
        GDIPlus() /* throw(Afx::Fault) */
        {
            // TODO: implementation
            ::DebugBreak();
        }

        ~GDIPlus() /* throw() */
        {
            // TODO: implementation
        }
    };

    struct WINSock
    {
        WINSock() /* throw(Afx::Fault) */
        {
            // TODO: implementation
            ::DebugBreak();
        }

        ~WINSock() /* throw() */
        {
            // TODO: implementation
        }
    };
}
#endif // 0
