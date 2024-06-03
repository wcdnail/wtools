#pragma once

#include <thread>
#include <memory>

namespace DH
{
    class DebugConsole;

    class DebugOutputListener
    {
    public:
        ~DebugOutputListener();
        DebugOutputListener(DebugConsole const& owner);

        bool Start();
        bool Stop();

    private:
        using   HandlePtr = std::shared_ptr<void>;
        using    ShmemPtr = std::shared_ptr<void>;

        DebugConsole const& owner_;
        HandlePtr        thrdStop_;
        HandlePtr        mutexPtr_;
        HandlePtr       buffReady_;
        HandlePtr       dataReady_;
        HandlePtr      mappingPtr_;
        ShmemPtr         shmemPtr_;
        HandlePtr securityDescPtr_;
        std::thread  thrdListener_;

        bool Init();
        void Listener() const;
    };
}
