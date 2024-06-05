#include "stdafx.h"
#include "dc.rich.edit.impl.h"
#include "tator/resource.h"
#include <fstream>

namespace DH
{
    struct DCRichEditImpl::StaticInit
    {
        static StaticInit& Instance()
        {
            static StaticInit staticInit;
            return staticInit;
        }

    private:
        HINSTANCE richEditLib_;

        ~StaticInit()
        {
            FreeLibrary(richEditLib_);
        }

        StaticInit()
            : richEditLib_{nullptr}
        {
            InitCommonControls();
            richEditLib_ = LoadLibrary(WTL::CRichEditCtrl::GetLibraryName());
        }
    };

    DCRichEditImpl::~DCRichEditImpl() = default;

    DCRichEditImpl::DCRichEditImpl(DebugConsole const& owner)
        : BasicDebugConsole{owner}
        ,    lastLineCount_{0}
    {
        UNREFERENCED_PARAMETER(StaticInit::Instance());
    }

    HRESULT DCRichEditImpl::PreCreateWindow()
    {
        static ATOM gs_dc2Atom{0};
        return CCustomControl::PreCreateWindowImpl(gs_dc2Atom, GetWndClassInfo());
    }

    HWND DCRichEditImpl::CreateConsole()
    {
        toolBox_ = WTL::CFrameWindowImplBase<>::CreateSimpleToolBarCtrl(m_hWnd, IDT_DH_DC2_TOOLBAR, FALSE,
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
            RBS_VARHEIGHT | RBS_BANDBORDERS | RBS_AUTOSIZE | 
            TBSTYLE_FLAT
        );

        CRect rc{};
        GetClientRect(rc);

        if (toolBox_.m_hWnd) {
            CRect rcToolBox;
            toolBox_.GetWindowRect(rcToolBox);
            rc.top += rcToolBox.Height();
        }

        console_.Create(m_hWnd, rc, nullptr, 
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
            ES_MULTILINE | ES_READONLY | ES_AUTOHSCROLL | ES_AUTOVSCROLL |
            ES_SUNKEN,
            0,
            ID_LOG_CTL
        );

        TEXTMODE tm = console_.GetTextMode();
        tm = (0 == (tm & TM_RICHTEXT)       ? static_cast<TEXTMODE>(tm | TM_RICHTEXT)        : tm);
        tm = (0 != (tm & TM_MULTILEVELUNDO) ? static_cast<TEXTMODE>(tm & ~TM_MULTILEVELUNDO) : tm);

        console_.SetTextMode(tm);
        console_.SetAutoURLDetect();

        CHARFORMAT2 charFmt{0};
        console_.GetDefaultCharFormat(charFmt);
        wcsncpy_s(charFmt.szFaceName, L"Cascadia Mono Light", _countof("Cascadia Mono Light"));
        console_.SetDefaultCharFormat(charFmt);
        
        //ATL::CComPtr<IRichEditOle> const richOle{console_.GetOleInterface()};
        //if (richOle) {
        //    ATL::CComQIPtr<ABI::Windows::UI::Text::ITextDocument> const textDoc{richOle};
        //    if (textDoc) {
        //        ATL::CComQIPtr<ABI::Windows::UI::Text::ITextCharacterFormat> charFmt{};
        //        textDoc->GetDefaultCharacterFormat(&charFmt);
        //        if (charFmt) {
        //            charFmt->put_Name(L"Courier New");
        //        }
        //
        //    }
        //}
        //console_.SetFont(consoleFont_.m_hFont, FALSE);

        return console_.m_hWnd;
    }

    void DCRichEditImpl::PreWrite()
    {
        int n = console_.GetWindowTextLength();
        console_.SetSel(n, n);
        lastLineCount_ = console_.GetLineCount();
    }

    void DCRichEditImpl::WriteNarrow(std::string& nrString, double, DWORD)
    {
        ::SendMessageA(console_, EM_REPLACESEL, (WPARAM)FALSE, reinterpret_cast<LPARAM>(nrString.c_str()));
    }

    void DCRichEditImpl::WriteWide(std::wstring& wdString, double, DWORD)
    {
        ::SendMessageW(console_, EM_REPLACESEL, (WPARAM)FALSE, reinterpret_cast<LPARAM>(wdString.c_str()));
    }

    void DCRichEditImpl::PostWrite()
    {
        if (GetParameters().autoScroll) {
            int const n{console_.GetLineCount() - lastLineCount_};
            console_.LineScroll(n);
        }
    }

    void DCRichEditImpl::OnCommand(UINT, int id, HWND)
    {
        if (ID_DC_BN_CLEAN == id) {
            Clean();
        }
        else if (ID_DC_BN_SAVE == id) {
            AskPathAndSave();
        }
    }

    void DCRichEditImpl::Save(char const* filePathName) const
    {
        std::streamsize const len{::GetWindowTextLengthA(console_)};
        if (len > 0) {
            // TODO: save rich text (RTF)!
            std::string buffer(static_cast<std::string::size_type>(len)+1, '\0');
            int const storedLen{::GetWindowTextA(console_, &buffer[0], static_cast<int>(len))};
            if (storedLen > 0) {
                std::ofstream output(filePathName);
                output << buffer.c_str() << std::flush;
            }
        }
    }

    void DCRichEditImpl::OnDestroy()
    {
        console_.DestroyWindow();
        if (toolBox_.m_hWnd) {
            toolBox_.DestroyWindow();
        }
    }
}
