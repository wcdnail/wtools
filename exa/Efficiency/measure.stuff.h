#pragma once

#include <cstdarg>
#include <fstream>
#include <iomanip>
#include <windows.h>
#include <atlstr.h>

namespace afx
{
    class _timer
    {
    public:
        _timer();
        ~_timer();
        double elapsed() const;

    private:
        double start_;
    };

    inline _timer::_timer()
        : start_(.0)
    {
        LARGE_INTEGER temp = {0};
        if (::QueryPerformanceCounter(&temp))
            start_ = static_cast<double>(temp.QuadPart);
    }

    inline _timer::~_timer()
    {}

    double _timer::elapsed() const
    {
        double stop = 0.0;
        double freq = 1.0;

        LARGE_INTEGER temp = {0};
        if (::QueryPerformanceCounter(&temp))
        {
            stop = static_cast<double>(temp.QuadPart);

            if (::QueryPerformanceFrequency(&temp))
                freq = static_cast<double>(temp.QuadPart);
        }

        return (freq == .0 ? .0 : ((stop - start_) / freq));
    }

    inline void _puts(char const* message)
    {
        ::OutputDebugStringA(message);
    }

    inline void _puts(wchar_t const* message)
    {
        ::OutputDebugStringW(message);
    }

    inline void _putsf(char const* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        {
            CStringA temp;
            temp.FormatV(format, ap);
            _puts(temp);
        }
        va_end(ap);
    }

    inline void _putsf(wchar_t const* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        {
            CStringW temp;
            temp.FormatV(format, ap);
            _puts(temp);
        }
        va_end(ap);
    }

    #define _meause(exp, tr)                                            \
        {                                                               \
            afx::_timer st;                                             \
            auto rv = exp;                                              \
            tr = st.elapsed();                                          \
            afx::_putsf("%.12f `%s` == `%s`\n"                          \
                      , tr                                              \
                      , #exp                                            \
                      , _ToString(rv).c_str()                           \
                      );                                                \
        }

    #define _measure_output(name)                                       \
        std::ofstream resultOutput(name)

    #define _measure_puts(text)                                         \
        resultOutput << text << std::endl

    #define _measure_storeresult(n)                                     \
        if (n > 0) {                                                    \
            CStringA temp;                                              \
            for (size_t j=0; j<_countof(resultTime); j++) {             \
                temp.Format("%.12f", resultTime[j]);                    \
                resultOutput << (char const*)temp << "\t";              \
            }                                                           \
            resultOutput << std::endl;                                  \
        }

    #define _measure3(n, e1, e2, e3)                                    \
        double resultTime[3] = {0};                                     \
        _meause(e1, resultTime[0])                                      \
        _meause(e2, resultTime[1])                                      \
        _meause(e3, resultTime[2])                                      \
        _measure_storeresult(n)

    #define _measure9(n, e1, e2, e3, e4, e5, e6, e7, e8, e9)            \
        double resultTime[9] = {0};                                     \
        _meause(e1, resultTime[0])                                      \
        _meause(e2, resultTime[1])                                      \
        _meause(e3, resultTime[2])                                      \
        _meause(e4, resultTime[3])                                      \
        _meause(e5, resultTime[4])                                      \
        _meause(e6, resultTime[5])                                      \
        _meause(e7, resultTime[6])                                      \
        _meause(e8, resultTime[7])                                      \
        _meause(e9, resultTime[8])                                      \
        _measure_storeresult(n)

    #define _measure_done()                                             \
        resultOutput << std::flush
}
