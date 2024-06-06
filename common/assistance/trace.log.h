#ifndef _assistance_trace_log_h__
#define _assistance_trace_log_h__

#include "trace.accumulator.h"
#include "trace.uptime.n.threadid.h"

namespace Trace
{
    class BasicLog
    {
    public:
        typedef Accumulator::String String;

        BasicLog();
        ~BasicLog();

        template <class U>
        void Put(U const& x);

        template <class Owner>
        void Flush();

    private:
        Accumulator Accum_;
    };

    inline BasicLog::BasicLog()
        : Accum_()
    {}

    inline BasicLog::~BasicLog()
    {}

    template <class U>
    inline void BasicLog::Put(U const& x)
    {
        Accum_.Put(x);
    }

    template <class Owner>
    inline void BasicLog::Flush()
    {
        Accumulator::Buffer log;

        try
        {
            String messages = Accum_.GetString();

            typedef boost::char_separator<wchar_t> Separator;
            typedef boost::tokenizer<Separator, String::const_iterator, String> Tokenizer;

            Separator separator(L"\n");
            Tokenizer tokens(messages, separator);

            for(Tokenizer::iterator it=tokens.begin(); it!=tokens.end(); ++it)
            {
                _UptimeNThreadId<wchar_t>(log);
                log << *it << std::endl;
            }

            Accum_.Clear();
        }
        catch (...)
        {
            Owner::Write(L"ERROR: BasicLog::Flush\n");
        }

        Owner::Write(log.str());
    }
}

#endif // _assistance_trace_log_h__
