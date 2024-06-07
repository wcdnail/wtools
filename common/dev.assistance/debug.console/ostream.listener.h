#pragma once

#include "debug.console.h"
#include <streambuf>

namespace DH
{
    template <class CT, class CHTraits = std::char_traits<CT>>
    class std_ostream_listener: public std::basic_streambuf<CT, CHTraits>
    {
    public:
        using     super = std::basic_streambuf<CT, CHTraits>;
        using stdstream = std::basic_ostream<CT, CHTraits>;
        using  int_type = typename super::int_type;

        std_ostream_listener(DebugConsole const& console, stdstream& str);
        ~std_ostream_listener() override;

        void install() const;
        void remove() const;
        void toggle(bool inst) const;

    private:
        DebugConsole const& console_;
        stdstream& stream_;
        super *last_;

        std::streamsize xsputn(CT const* string, std::streamsize len) override;
        int_type overflow(int_type v) override;
    };

    template <class CT, class CHTraits>
    std_ostream_listener<CT, CHTraits>::std_ostream_listener(DebugConsole const& console, stdstream& str)
        : console_(console)
        , stream_(str)
        , last_(str.rdbuf())
    {
    }

    template <class CT, class CHTraits>
    std_ostream_listener<CT, CHTraits>::~std_ostream_listener()
    {
    }

    template <class CT, class CHTraits>
    inline void std_ostream_listener<CT, CHTraits>::install() const
    {
        stream_.rdbuf(const_cast<std_ostream_listener<CT, CHTraits>*>(this));
    }

    template <class CT, class CHTraits>
    inline void std_ostream_listener<CT, CHTraits>::remove() const
    {
        stream_.rdbuf(last_);
    }

    template <class CT, class CHTraits>
    inline void std_ostream_listener<CT, CHTraits>::toggle(bool inst) const
    {
        (inst ? install() : remove());
    }

    template <class CT, class CHTraits>
    inline std::streamsize std_ostream_listener<CT, CHTraits>::xsputn(CT const* string, std::streamsize len)
    {
        console_.Puts<CT>(0, std::basic_string_view<CT>{string, static_cast<size_t>(len)});
        return len;
    }

    template <class CT, class CHTraits>
    inline typename std_ostream_listener<CT, CHTraits>::int_type std_ostream_listener<CT, CHTraits>::overflow(int_type v)
    {
        CT const ch[2] = { static_cast<CT>(v), 0 };
        console_.Puts<CT>(0, std::basic_string_view<CT>{ch, static_cast<size_t>(1)});
        return v;
    }
}
