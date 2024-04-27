#pragma once

#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/cstdint.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace Fv
{
    typedef boost::filesystem::path Path;
    typedef boost::system::error_code ErrorCode;
    typedef boost::uint64_t SizeType;
    typedef boost::shared_ptr<void> AutoHandle;
    typedef unsigned char Byte;
}
