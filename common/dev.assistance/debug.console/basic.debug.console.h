#pragma once

#include "debug.output.listener.h"
#include "ostream.listener.h"
#include <WTL/CCustomCtrl.h>
#include <utility>
#include <string>
#include <mutex>
#include <deque>

namespace DH
{
    class DebugConsole;

    class BasicDebugConsole: protected CCustomControl<BasicDebugConsole>,
                             private   WTL::CDialogResize<BasicDebugConsole>
    {
        friend WndSuper;
        friend CCustomControl<BasicDebugConsole>;
        friend WTL::CDialogResize<BasicDebugConsole>;

    protected:
        BasicDebugConsole(DebugConsole const& owner);

    public:
        static constexpr DWORD dwCurrentPID{static_cast<DWORD>(-1)};

        enum { WM_SYNC_STRINGS = WM_USER + 1 };

        using WndSuper::m_hWnd;
        using WndSuper::GetClientRect;
        using WndSuper::ShowWindow;
        using WndSuper::UpdateWindow;
        using WndSuper::Create;
        using WndSuper::DestroyWindow;

        using StringPair = std::pair<std::string, std::wstring>;

        struct StringItem
        {
            StringPair pair_;
            DWORD       pid_;
            double       ts_;

            ~StringItem();
            StringItem(StringPair&& pair, DWORD dwPID);
            StringItem(std::string_view nrView, DWORD dwPID);
            StringItem(std::wstring_view wdView, DWORD dwPID);

            std::wstring GetText() const;
        };

        using StringQue = std::deque<StringItem>;

        struct Parameters
        {
            ~Parameters();
            Parameters(char const* fn, int fs, int a, int w, int h);

            char const* fname; // шрифт
            int fsize;         // размер шрифта
            int align;         // позиция окна на экране
            int cx;            // ширина
            int cy;            // высота
            bool autoScroll;
        };

        enum { ID_LOG_CTL = 87621 };

        ~BasicDebugConsole() override;

        virtual HRESULT PreCreateWindow() = 0;

        void CreateWindowIfNessesary();
        void PutWindow();
        void SetupFont();

        Parameters& GetParameters();
        void SetParameters(int cx, int cy, int align, int fsize, char const* fname);

        void PutsNarrow(std::string_view nrView, DWORD dwPID);
        void PutsWide(std::wstring_view wdView, DWORD dwPID);
        void FormatVNarrow(DWORD dwPID, std::string_view nrFormat, va_list vaList);
        void FormatVWide(DWORD dwPID, std::wstring_view nrFormat, va_list vaList);
        StringQue const& GetCache() const;
        virtual void Clean() const;

        void ReceiveStdOutput(bool on) const;
        bool ReceiveDebugOutput(PCWSTR pszWindowName, bool bGlobal);

        void AskPathAndSave() const;
        virtual void Save(char const* filePathName) const;

        static std::string GenerateLogFilename();

        BOOL SubclassWindow(HWND hWnd);

    private:
        BEGIN_DLGRESIZE_MAP(BasicDebugConsole)
            DLGRESIZE_CONTROL(ID_LOG_CTL, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        END_DLGRESIZE_MAP()

        BEGIN_MSG_MAP(BasicDebugConsole)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroyNative)
            MSG_WM_COMMAND(OnCommand)
            MESSAGE_HANDLER_EX(WM_SYNC_STRINGS, OnSyncStrings)
            CHAIN_MSG_MAP(CDialogResize<BasicDebugConsole>)
        END_MSG_MAP()

    protected:
        Parameters                           params_;
        WTL::CFont                      consoleFont_;
        HWND                          consoleHandle_;
        std_ostream_listener<char>     coutListener_;
        std_ostream_listener<char>     cerrListener_;
        std_ostream_listener<wchar_t> wcoutListener_;
        std_ostream_listener<wchar_t> wcerrListener_;
        DebugOutputListener     debugOutputListener_;
        StringQue                             cache_;
        std::recursive_mutex                cacheMx_;

        virtual HWND CreateConsole() = 0;

        virtual void PreWrite() = 0;
        virtual void WriteNarrow(std::string&, double, DWORD) = 0;
        virtual void WriteWide(std::wstring&, double, DWORD) = 0;
        virtual void PostWrite();

        virtual void OnCommand(UINT notifyCode, int id, HWND);
        virtual void OnDestroy();

        int OnCreate(LPCREATESTRUCT);
        void OnDestroyNative();
        void LoadStringsFromCache();
        LRESULT OnSyncStrings(UINT = 0, WPARAM = 0, LPARAM = 0);
    };

    inline BasicDebugConsole::Parameters::Parameters(char const* fn, int fs, int a, int w, int h)
        : fname(fn)
        , fsize(fs)
        , align(a)
        , cx(w)
        , cy(h)
        , autoScroll(true)
    {
    }

    inline BasicDebugConsole::StringQue const& BasicDebugConsole::GetCache() const
    {
        return cache_;
    }
}
