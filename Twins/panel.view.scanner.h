#pragma once

#include "panel.view.item.h"
#include <dir.manager.h>
#include <settings.h>
#include <boost/noncopyable.hpp>
#include <cstdint>
#include <thread>
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace Twins
{
    class FView;

    struct FScanner: boost::noncopyable
    {
        using  Callback = std::function<void(double)>;
        using DataArray = std::unique_ptr<uint8_t[]>;

        FScanner();
        void Spawn();
        ~FScanner();

        void Cancel();
        void Fetch(PCWSTR path = nullptr);
        void UpdatePath(std::wstring const& line);

        CF::DirManager            Path;
        std::wstring              Mask;
        FItemVector              Items;
        Callback           OnFetchDone;
        volatile bool       InProgress;
        bool                   IsError;
        HRESULT              ErrorCode;
        std::wstring         ErrorText;
        std::wstring      SelectedName;
        bool             DisplayHidden;

    private:
        DataArray                 Data;
        volatile bool       AbortFetch;
        std::thread             Thread;

        static size_t   infoCacheSize_;
        static Conf::Section settings_;

        enum
        {
            FlagStop = WAIT_OBJECT_0,
            FlagFetch,
            FlagCount
        };

        HANDLE Flag[FlagCount];

        void HeavyDutyProc();
        void ParseBuffer();

        struct InternalError
        {
            HRESULT             Hr;
            wchar_t const* Message;
        };

        static InternalError ReadDir(PCWSTR path, uint8_t buffer[], size_t bsize);
    };
}
