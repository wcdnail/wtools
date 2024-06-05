#pragma once

#include <thread>
#include <memory>

namespace DH
{
    class DebugConsole;
    class BasicDebugConsole;

    class DebugOutputListener
    {
        friend class BasicDebugConsole;

        ~DebugOutputListener();
        DebugOutputListener(DebugConsole const& rMaster);

        bool Start(PCWSTR pszWindowName, bool bGlobal);
        bool Stop();

        using HandlePtr = std::shared_ptr<void>;

        DebugConsole const& master_;
        HandlePtr         thrdStop_;
        HandlePtr       mappingPtr_;
        HandlePtr        buffReady_;
        HandlePtr        dataReady_;
        HandlePtr         shmemPtr_;
        std::thread   thrdListener_;

        bool Init(bool bGlobal, PCWSTR pszWindowName = nullptr);
        void Listener() const;
    };
}
