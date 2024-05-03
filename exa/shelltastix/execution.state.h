#pragma once

namespace Shelltastix
{
    class ExecutionState
    {
    public:
        void Start();
        void Stop();
        void Cancel();
        bool Canceled() const;
        bool Busy() const;

    private:
        enum States
        {
            Idle = -1,
            Executing,
            StopPending,
            Stoped,
            ItIs,
        };

        volatile LONG state_;

    private:
        friend ExecutionState& Execution();

        ExecutionState();
        ~ExecutionState();
    };

    inline ExecutionState::ExecutionState()
        : state_(Idle)
    {}

    inline ExecutionState::~ExecutionState()
    {}

    inline bool ExecutionState::Canceled() const
    {
        volatile LONG prev = -1;

        ::InterlockedExchange(&prev, state_);
        ::InterlockedCompareExchange(&prev, ItIs, StopPending);

        return ItIs == prev;
    }

    inline bool ExecutionState::Busy() const
    {
        volatile LONG prev = -1;

        ::InterlockedExchange(&prev, state_);
        ::InterlockedCompareExchange(&prev, ItIs, Executing);

        return ItIs == prev;
    }

    inline void ExecutionState::Start()
    {
        ::InterlockedExchange(&state_, Executing);
    }

    inline void ExecutionState::Stop()
    {
        ::InterlockedExchange(&state_, Stoped);
    }

    inline void ExecutionState::Cancel()
    {
        ::InterlockedExchange(&state_, StopPending);
    }

    inline ExecutionState& Execution()
    {
        static ExecutionState instance;
        return instance;
    }
}
