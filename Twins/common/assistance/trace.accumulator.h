#ifndef _assistance_trace_accumulator_h__
#define _assistance_trace_accumulator_h__

#include <boost/noncopyable.hpp>
#include <sstream>
#include <xutility>
#include <iterator>

namespace Trace
{
    class Accumulator: boost::noncopyable
    {
    public:
        typedef std::wstring String;
        typedef std::wostringstream Buffer;

        Accumulator();
        ~Accumulator();

        template <class T>
        void Put(T const& x);
        void Clear();
        String GetString() const;
       
    private:
        Buffer Buffer_;
    };

    inline Accumulator::Accumulator() 
        : Buffer_()
    {}

    inline Accumulator::~Accumulator() 
    {}

    template <class T>
    inline void Accumulator::Put(T const& x)
    {
        try
        {
            Buffer_ << x;
        }
        catch (...)
        {
            // TODO: handle error.
        }
    }

    inline void Accumulator::Clear()
    {
        Buffer clean;
        Buffer_.swap(clean);
    }

    inline Accumulator::String Accumulator::GetString() const 
    {
        return Buffer_.str(); 
    }
}

#endif // _assistance_trace_accumulator_h__
