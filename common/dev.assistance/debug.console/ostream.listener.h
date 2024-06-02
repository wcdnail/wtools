#pragma once

#include <streambuf>

namespace DH
{
    template <class CT, class _Traits = std::char_traits<CT> >
    class std_ostream_listener: public std::basic_streambuf<CT, _Traits>
    {
    public:
        using     super = std::basic_streambuf<CT, _Traits>;
        using stdstream = std::basic_ostream<CT, _Traits>;
        using  int_type = super::int_type;

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

    template <class CT, class _Traits>
    std_ostream_listener<CT, _Traits>::std_ostream_listener(DebugConsole const& console, stdstream& str)
        : console_(console)
        , stream_(str)
        , last_(str.rdbuf())
    {
    }

    template <class CT, class _Traits>
    std_ostream_listener<CT, _Traits>::~std_ostream_listener()
    {
    }

    template <class CT, class _Traits>
    inline void std_ostream_listener<CT, _Traits>::install() const
    {
        stream_.rdbuf(const_cast<std_ostream_listener<CT, _Traits>*>(this));
    }

    template <class CT, class _Traits>
    inline void std_ostream_listener<CT, _Traits>::remove() const
    {
        stream_.rdbuf(last_);
    }

    template <class CT, class _Traits>
    inline void std_ostream_listener<CT, _Traits>::toggle(bool inst) const
    {
        (inst ? install() : remove());
    }

    template <class CT, class _Traits>
    inline std::streamsize std_ostream_listener<CT, _Traits>::xsputn(CT const* string, std::streamsize len)
    {
        console_.Puts(string);
        return len;
    }

    template <class CT, class _Traits>
    inline typename std_ostream_listener<CT, _Traits>::int_type std_ostream_listener<CT, _Traits>::overflow(int_type v)
    {
        TCHAR ch[2] = { (TCHAR)v, 0 };
        console_.Puts(ch);
        return v;
    }
}
