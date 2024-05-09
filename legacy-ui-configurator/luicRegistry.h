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
    ValueType GetValue(StrView name, ValueType defaultValue) const;

private:
    HKEY     m_hKey;
    bool m_bCreated;

    bool GetValueImpl(StrView name, void* data, size_t dataSize, DWORD& dwType) const;
};

template <typename ValueType>
inline ValueType CRegistry::GetValue(StrView name, ValueType defaultValue) const
{
    DWORD     dwType{0};
    ValueType result{};
    if (GetValueImpl(name, static_cast<void*>(&result), sizeof(result), dwType)) {
        return result;
    }
    return defaultValue;
}
