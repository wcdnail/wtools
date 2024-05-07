#ifndef _cf_exceptions_any_unittest_h__
#define _cf_exceptions_any_unittest_h__

#ifdef _MSC_VER
#pragma once
#endif

#include "exceptions.any.h"
#include "dh.tracing.h"

namespace CF
{
    namespace ExceptionAny
    {
        namespace
        {
            inline void OnException(char const* caption, char const* what, char const* codetext, char const* callstack)
            {
                DH::TPrintf(DH::Category::Exception "%s:\n%s\n%s\n%s\n", caption, what, codetext, callstack);
            }
        }

        inline void RunTest()
        {
            try
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("Test `BOOST_THROW_EXCEPTION`"));
            }
            catch (...)
            {
                CF::HandleAnyException(OnException);
            }

            try
            {
                throw std::runtime_error("Test `std::runtime_error`");
            }
            catch (...)
            {
                CF::HandleAnyException(OnException);
            }
        }
    }
}

#endif // _cf_exceptions_any_unittest_h__
