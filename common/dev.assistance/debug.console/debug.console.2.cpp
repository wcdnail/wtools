#include "stdafx.h"
#include "debug.console.2.h"
#include "../debug.console/basic.debug.console.h"
#include <fstream>
#include "resources/resource.h"

namespace Dh
{
    class DCRichEditImpl: public BasicDebugConsole
    {
    public:
        DCRichEditImpl(DebugConsole const& owner);
        virtual ~DCRichEditImpl();

    private:
        CRichEditCtrl console_;
        HINSTANCE richEditLib_;
        CToolBarCtrl toolBox_;
        int lastLineCount_;

        virtual void Save(char const* filePathName) const;

        virtual HWND CreateConsole();
        virtual void PreWrite();
        virtual void WriteString(char const*);
        virtual void WriteString(wchar_t const*);
        virtual void PostWrite();
        virtual void OnDestroy();
        
        virtual void OnCommand(UINT notifyCode, int id, HWND);
    };

    DCRichEditImpl::DCRichEditImpl(DebugConsole const& owner)
        : BasicDebugConsole(owner)
        , richEditLib_(NULL)
        , lastLineCount_(0)
    {
        InitCommonControls();
        richEditLib_ = ::LoadLibrary(CRichEditCtrl::GetLibraryName());
    }

    DCRichEditImpl::~DCRichEditImpl()
    {
        FreeLibrary(richEditLib_);
    }

    HWND DCRichEditImpl::CreateConsole()
    {
        toolBox_ = CFrameWindowImplBase<>::CreateSimpleToolBarCtrl(m_hWnd, IDT_DH_DC2_TOOLBAR, FALSE
            , WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS 
            | RBS_VARHEIGHT | RBS_BANDBORDERS | RBS_AUTOSIZE
            | TBSTYLE_FLAT
            );

        CRect rc;
        GetClientRect(rc);

        if (toolBox_.m_hWnd)
        {
            CRect rcToolBox;
            toolBox_.GetWindowRect(rcToolBox);
            rc.top += rcToolBox.Height();
        }

        console_.Create(m_hWnd, rc, NULL
            , WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL 
            | ES_MULTILINE | ES_READONLY | ES_AUTOHSCROLL | ES_AUTOVSCROLL
            | ES_SUNKEN
            , 0
            , ID_LOG_CTL);

        TEXTMODE tm = console_.GetTextMode();
        tm = (0 == (tm & TM_RICHTEXT)       ? (TEXTMODE)(tm | TM_RICHTEXT)        : tm);  
        tm = (0 != (tm & TM_MULTILEVELUNDO) ? (TEXTMODE)(tm & ~TM_MULTILEVELUNDO) : tm);  

        //console_.SetBackgroundColor(::GetSysColor(COLOR_WINDOW-1));
        console_.SetTextMode(tm);
        console_.SetAutoURLDetect();

        return console_.m_hWnd;
    }

    void DCRichEditImpl::PreWrite()
    {
        int n = console_.GetWindowTextLength();
        console_.SetSel(n, n);
        lastLineCount_ = console_.GetLineCount();
    }

    void DCRichEditImpl::WriteString(char const* string)
    {
        ::SendMessageA(console_, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)string);
    }

    void DCRichEditImpl::WriteString(wchar_t const* string)
    {
        ::SendMessageW(console_, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)string);
    }

    void DCRichEditImpl::PostWrite()
    {
        if (GetParameters().autoScroll)
        {
            int n = console_.GetLineCount() - lastLineCount_;
            console_.LineScroll(n);
        }
    }

    void DCRichEditImpl::OnCommand(UINT, int id, HWND)
    {
        if (ID_DC_BN_CLEAN == id)
        {
            Clean();
        }
        else if (ID_DC_BN_SAVE == id)
        {
            AskPathAndSave();
        }
    }

    void DCRichEditImpl::Save(char const* filePathName) const
    {
        std::streamsize len = (std::streamsize)::GetWindowTextLengthA(console_);
        if (len > 0)
        {
            // TODO: save rich text (RTF)!
            std::string buffer((std::string::size_type)len+1, '\0');
            int storedLen = ::GetWindowTextA(console_, &buffer[0], (int)len);
            if ( storedLen > 0 )
            {
                std::ofstream output(filePathName);
                output << buffer.c_str() << std::flush;
            }
        }
    }

    void DCRichEditImpl::OnDestroy()
    {
        console_.DestroyWindow();

        if (toolBox_.m_hWnd)
        {
            toolBox_.DestroyWindow();
        }
    }

    DebugConsole2& DebugConsole2::Instance()
    {
        static DebugConsole2 instance;
        return instance;
    }

    DebugConsole2::DebugConsole2()
        : DebugConsole(new DCRichEditImpl(*this))
    {
    }

    DebugConsole2::~DebugConsole2()
    {
    }
}
