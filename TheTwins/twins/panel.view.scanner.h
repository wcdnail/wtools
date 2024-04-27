#pragma once

#include "panel.view.item.h"
#include <dir.manager.h>
#include <settings.h>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/scoped_array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <vector>
#include <string>

namespace Twins
{
    class FView;

    struct FScanner: boost::noncopyable
    {
        typedef boost::function<void(double)> Callback;

        FScanner();
        ~FScanner();

        void Cancel();
        void Fetch(wchar_t const* path = NULL);
        void UpdatePath(std::wstring const& line);

        Cf::DirManager Path;
        std::wstring Mask;
        FItemVector Items;
        Callback OnFetchDone;
        volatile bool InProgress;
        bool IsError;
        HRESULT ErrorCode;
        std::wstring ErrorText;
        std::wstring SelectedName;
        bool DisplayHidden;

    private:
        boost::scoped_array<unsigned char> Data;
        volatile bool AbortFetch;
        boost::thread Thread;

        static size_t Ds;
        static Conf::Section ClassSettings;

        enum 
        { FlagStop = 0
        , FlagFetch
        , FlagCount 
        };

        HANDLE Flag[FlagCount];

        void HeavyDutyProc();
        void ParseBuffer();

        static void ReadDir(wchar_t const* path, unsigned char* buffer, size_t bsize);
    };
}
