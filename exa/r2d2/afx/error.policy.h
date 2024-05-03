#ifndef aw_error_policy_h__
#define aw_error_policy_h__

#include <string>
#include <sstream>
#include <boost/exception/all.hpp>
#include <boost/shared_ptr.hpp>

class Exception: public std::exception, public boost::exception
{
public:
    typedef boost::error_info<struct Filename, std::string> Filename;
    typedef boost::error_info<struct Message, std::string> Message;
    typedef boost::error_info<struct Function, std::string> Function;
    typedef boost::error_info<struct ErrorCode, unsigned> ErrorCode;
    typedef boost::error_info<struct Errorno, int> Errorno;

    Exception(char const* info) throw()
        : std::exception(info) 
    {}

    ~Exception() throw() {}

    char const* Exception::Dump() const throw()
    {
        DebugDump();

        try
        {
            std::ostringstream temp;
            temp << what();
            if (char const *s = boost::exception_detail::get_diagnostic_information(*this, ""))
                if(*s)
                    temp << "\r\n" << s << "\r\n";
            dump_ = temp.str(); 
        }
        catch(...) {}

        return dump_.c_str();
    }


    static std::string ErrnoToString(int code)
    {
        std::ostringstream s;
        s << code << " (" << strerror(code) << ")";
        return s.str();
    }

private:
    void DebugDump() const throw();

    mutable std::string dump_;
};

inline std::string to_string(Exception::Errorno const& e)
{
    return Exception::ErrnoToString(e.value());
}

class DllExceptionPolicy
{
public:
    template <class C>
    static void LoadFailed(C const* name, int code) 
    {
        BOOST_THROW_EXCEPTION(Exception("Can't load module")
            << Exception::Filename(name)
            << Exception::ErrorCode(code)
            );
    }

    static void FunctionNotFound(char const* name, int code)
    {
        BOOST_THROW_EXCEPTION(Exception("Function not found") 
            << Exception::Function(name)
            << Exception::ErrorCode(code)
            );
    }
};

#endif // aw_error_policy_h__
