#pragma once

#include <cstdint>
#include <filesystem>
#include <system_error>
#include <memory>

namespace Fv
{
    typedef std::filesystem::path Path;
    typedef std::error_code ErrorCode;
    typedef uint64_t SizeType;
    typedef std::shared_ptr<void> AutoHandle;
    typedef unsigned char Byte;
}
