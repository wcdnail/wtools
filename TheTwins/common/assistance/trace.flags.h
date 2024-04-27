#ifndef _assistance_trace_flags_h__
#define _assistance_trace_flags_h__

namespace Trace
{
    struct ControlFlag 
    {
        enum
        {
            None        = 0x00000000,
            EndOfLine   = 0x00000001,
            Purge       = 0x00000002,
        };

        ControlFlag(unsigned initial = None)
            : Flags_(initial) 
        {}

        bool IsOn(unsigned mask) const
        {
            return 0 != (Flags_ & mask);
        }

    private:
        unsigned Flags_;
    };

    static const ControlFlag Eofl = ControlFlag(ControlFlag::EndOfLine);
    static const ControlFlag Endl = ControlFlag(ControlFlag::EndOfLine | ControlFlag::Purge);
    static const ControlFlag Flush = ControlFlag(ControlFlag::Purge);
}

#endif // _assistance_trace_flags_h__
