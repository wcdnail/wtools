#pragma once

#include <boost/noncopyable.hpp>
#include <cstdint>
#include <filesystem>
#include <memory>

namespace Fv
{
    typedef std::filesystem::path Path;
    typedef std::error_code ErrorCode;
    typedef uint64_t SizeType;
    typedef std::shared_ptr<void> AutoHandle;
    typedef unsigned char Byte;
}
