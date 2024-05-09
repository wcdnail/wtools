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

    ~CRegistry();
    CRegistry();
    CRegistry(HKEY hRootKey, StrView path = {});

    bool Open(HKEY hRootKey, StrView path = {});
    int ForEachValue(ForEachFn const& frRoutine) const;

    bool IsOk() const { return nullptr != m_hKey; }

    template <typename ValueType>
    bool GetValue(StrView name, ValueType& value) const;

private:
    HKEY     m_hKey;
    bool m_bCreated;

    bool GetValueImpl(StrView name, void* data, size_t dataSize) const;
};

template <typename ValueType>
inline bool CRegistry::GetValue(StrView name, ValueType& value) const
{
    return GetValueImpl(name, static_cast<void*>(&value), sizeof(value));
}
