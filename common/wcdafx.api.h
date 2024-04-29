#pragma once

#if defined(WCDAFX_DLL_BUILD_CF)
#   define WCDAFX_API __declspec(dllexport)
#elif defined(WCDAFX_DLL)
#   define WCDAFX_API __declspec(dllimport)
#else
#   define WCDAFX_API
#endif

#ifdef __cplusplus
#  define DELETE_COPY_OF(theClass)                      \
          theClass(theClass const&) = delete;            \
          theClass& operator = (theClass const&) = delete

#  define DELETE_MOVE_OF(theClass)                      \
          theClass(theClass&&) = delete;                 \
          theClass& operator = (theClass&&) = delete

#  define DELETE_COPY_MOVE_OF(theClass) \
          DELETE_COPY_OF(theClass);      \
          DELETE_MOVE_OF(theClass)

#  define DECL_COPY_OF(theClass)                           \
          WCDAFX_API theClass(theClass const&);             \
          WCDAFX_API theClass& operator = (theClass const&)

#  define DECL_MOVE_OF(theClass)                      \
          WCDAFX_API theClass(theClass&&);             \
          WCDAFX_API theClass& operator = (theClass&&)

#  define DECL_COPY_MOVE_OF(theClass)  \
          DECL_COPY_OF(theClass);       \
          DECL_MOVE_OF(theClass)

#  define IMPL_DEFAULT_COPY_OF(theClass)                            \
          theClass::theClass(theClass const&) = default;             \
          theClass& theClass::operator = (theClass const&) = default

#  define IMPL_DEFAULT_MOVE_OF(theClass)                       \
          theClass::theClass(theClass&&) = default;             \
          theClass& theClass::operator = (theClass&&) = default

#  define IMPL_DEFAULT_COPY_MOVE_OF(theClass) \
          IMPL_DEFAULT_COPY_OF(theClass);      \
          IMPL_DEFAULT_MOVE_OF(theClass)

#endif