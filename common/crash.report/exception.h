#pragma once

#ifdef __cplusplus
#include "wcdafx.api.h"
#include "string.hp.h"
#include <stdexcept>

struct RUN_CTX;
typedef struct RUN_CTX* PRUN_CONTEXT;
typedef struct RUN_CTX const* PCRUN_CONTEXT;

namespace Crash
{
    class Exception: public std::runtime_error
    {
    public:
        using Super = std::runtime_error;

        WCDAFX_API ~Exception() override;
        WCDAFX_API Exception(CharString const& message, char const* file, int line, int ec = errno);
        WCDAFX_API Exception(WidecharString const& message, char const* file, int line, int ec = errno);

        WCDAFX_API char const* GetSource() const;
        WCDAFX_API int GetLine() const;
        WCDAFX_API int GetErrorCode() const;
        WCDAFX_API PCRUN_CONTEXT GetRunContext() const;

    private:
        char const*        File;
        int                Line;
        int           ErrorCode;
        PRUN_CONTEXT RunContext;
    };
}

#define CrashException(M, C) Crash::Exception(M, __FILE__, __LINE__, C)
#endif /* __cplusplus */
