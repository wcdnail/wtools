#pragma once

#include <wcdafx.api.h>
#include <string_view>
#include <functional>

struct CRegistry
{
    DELETE_COPY_MOVE_OF(CRegistry);

    static constexpr int ResultFail = -1;

    using   StrView = std::wstring_view;
    using ForEachFn = std::function<bool(HKEY hKey, PCWSTR szName, int nNameLen)>;

    WCDAFX_API ~CRegistry();
    WCDAFX_API CRegistry();
    WCDAFX_API CRegistry(HKEY hRootKey, REGSAM samDesired, StrView path = {});

    WCDAFX_API bool Open(HKEY hRootKey, REGSAM samDesired, StrView path = {});
    WCDAFX_API int ForEachValue(ForEachFn const& frRoutine) const;

    bool IsOk() const { return nullptr != m_hKey; }

    template <typename ValueType>
    bool GetValue(StrView name, ValueType& value) const;

    template <typename ValueType, size_t Count>
    bool GetArray(StrView name, ValueType (&arr)[Count]) const;

private:
    HKEY     m_hKey;
    bool m_bCreated;

    WCDAFX_API bool GetValueImpl(StrView name, void* data, size_t dataSize) const;
};

template <typename ValueType>
inline bool CRegistry::GetValue(StrView name, ValueType& value) const
{
    return GetValueImpl(name, static_cast<void*>(&value), sizeof(value));
}

template <typename ValueType, size_t Count>
inline bool CRegistry::GetArray(StrView name, ValueType (&arr)[Count]) const
{
    return GetValueImpl(name, static_cast<void*>(&arr), Count * sizeof(ValueType));
}
