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
        DELETE_COPY_MOVE_OF(DebugConsole);

        WCDAFX_API static DebugConsole& Instance();

        WCDAFX_API HRESULT Initialize() const;

        WCDAFX_API void ReceiveDebugOutput(bool on, PCWSTR pszWindowName, bool bGlobal) const;
        WCDAFX_API void ReceiveStdOutput(bool on) const;

        WCDAFX_API void SetParameters(int cx, int cy, int align, int fontSize, char const* fontName) const;
        WCDAFX_API void SetAutoScroll(bool on) const;

        WCDAFX_API void Show() const;
        WCDAFX_API void Hide() const;
        WCDAFX_API void Clean() const;

        WCDAFX_API void SetTitleText(char const*) const;
        WCDAFX_API void SetTitleText(wchar_t const*) const;

        WCDAFX_API void PutsNarrow(std::string_view) const;
        WCDAFX_API void PutsWide(std::wstring_view) const;

        WCDAFX_API void AskPathAndSave() const;
        WCDAFX_API void Save(char const* filePathName) const;

        WCDAFX_API void Destroy() const;

        template <class CT>
        void Puts(std::basic_string_view<CT> tView) const;

    protected:
        DebugConsole(std::unique_ptr<BasicDebugConsole>&&);
        ~DebugConsole();

    private:
        std::unique_ptr<BasicDebugConsole> impl_;

        DebugConsole();

        friend class DebugOutputListener;
        void* GetConsoleSystemHandle() const;
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
