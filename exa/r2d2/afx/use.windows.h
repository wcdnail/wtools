#ifndef aw_use_windows_h__
#define aw_use_windows_h__

#include <windows.h>
#include <wincon.h>
#include <tchar.h>
#include "error.policy.h"

inline std::string to_string(Exception::ErrorCode const& e)
{
    unsigned code = e.value();
    std::ostringstream result;
    result << "0x" << std::hex << code;

    char buf[2048] = {0};
    int len = (int)FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, code, 0, buf, sizeof(buf), NULL);
    if (len > 0) 
    {
        buf[len-2] = 0;
        result << " (" << buf << ")";
    }

    return result.str();
}

inline void Exception::DebugDump() const throw()
{
    try 
    {
        std::string dump("\n<< Exception info >>\n");
        dump += diagnostic_information(*this);
        dump += "\n\n";
        OutputDebugStringA(dump.c_str());
    }
    catch(...)
    {}
}

#endif // aw_use_windows_h__
