#pragma once

#include <wcdafx.api.h>
#include <memory>

namespace DH
{
    class DebugOutputListener;
    class BasicDebugConsole;

    class DebugConsole
    {
    public:
        using HandlePtr = std::shared_ptr<void>;

        static constexpr DWORD dwCurrentPID{static_cast<DWORD>(-1)};

        DELETE_COPY_MOVE_OF(DebugConsole);

        WCDAFX_API static DebugConsole& Instance();

        WCDAFX_API bool AdjustPrivileges() const;
        WCDAFX_API bool Create(HWND hWndParent, CRect& rc, DWORD dwStyle, DWORD dwExStyle, UINT nID) const;
        WCDAFX_API HRESULT Initialize() const;

        WCDAFX_API bool ReceiveDebugOutput(PCWSTR pszWindowName, bool bGlobal) const;
        WCDAFX_API void ReceiveStdOutput(bool on) const;

        WCDAFX_API void SetParameters(int cx, int cy, int align, int fontSize, char const* fontName) const;
        WCDAFX_API void SetAutoScroll(bool on) const;

        WCDAFX_API void Show() const;
        WCDAFX_API void Hide() const;
        WCDAFX_API void Clean() const;

        WCDAFX_API void SetTitleText(char const*) const;
        WCDAFX_API void SetTitleText(wchar_t const*) const;

        WCDAFX_API void PutsNarrow(std::string_view, DWORD dwPID = dwCurrentPID) const;
        WCDAFX_API void PutsWide(std::wstring_view, DWORD dwPID = dwCurrentPID) const;
        WCDAFX_API void FormatVNarrow(DWORD dwPID, std::string_view nrFormat, va_list vaList) const;
        WCDAFX_API void FormatVWide(DWORD dwPID, std::wstring_view wdFormat, va_list vaList) const;
        WCDAFX_API void FormatNarrow(DWORD dwPID, std::string_view nrFormat, ...) const;
        WCDAFX_API void FormatWide(DWORD dwPID, std::wstring_view wdFormat, ...) const;
        WCDAFX_API std::wstring GetStrings(std::wstring_view svSeparator) const;

        WCDAFX_API void AskPathAndSave() const;
        WCDAFX_API void Save(char const* filePathName) const;

        WCDAFX_API void Destroy() const;

        template <class CT>
        void Puts(std::basic_string_view<CT> tView) const;

        WCDAFX_API BOOL SubclassWindow(HWND hWnd) const;

    protected:
        DebugConsole(std::unique_ptr<BasicDebugConsole>&&);
        ~DebugConsole();

    private:
        std::unique_ptr<BasicDebugConsole> impl_;

        DebugConsole();

        static bool SetPrivilege(HANDLE hToken, PCWSTR pszPrivilege, bool bEnable);
    };

    template <>
    inline void DebugConsole::Puts<char>(std::basic_string_view<char> nrView) const
    {
        this->PutsNarrow(nrView);
    }

    template <>
    inline void DebugConsole::Puts<wchar_t>(std::basic_string_view<wchar_t> wdView) const
    {
        this->PutsWide(wdView);
    }
}
