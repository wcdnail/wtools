#include "stdafx.h"
#include "dc.listview.impl.h"
#include <string.utils.error.code.h>
#include <dh.tracing.h>
#include <dh.tracing.defs.h>
#include <fstream>

namespace 
{
    enum DCColimnIndex: int
    {
        DCCI_Numba = 0,
        DCCI_TS,
        DCCI_Text,
        DCCI_COUNT
    };
}

namespace DH
{
    struct DCListViewImpl::StaticInit
    {
        static StaticInit& Instance()
        {
            static StaticInit staticInit;
            return staticInit;
        }

    private:
        ~StaticInit() {}
        StaticInit() {}
    };

    DCListViewImpl::~DCListViewImpl() = default;

    DCListViewImpl::DCListViewImpl(DebugConsole const& owner)
        : BasicDebugConsole{owner}
        ,          console_{}
        ,          toolBox_{}
    {
        UNREFERENCED_PARAMETER(StaticInit::Instance());
    }

    HRESULT DCListViewImpl::PreCreateWindow()
    {
        static ATOM gs_dc2Atom{0};
        return PreCreateWindowImpl(gs_dc2Atom, GetWndClassInfo());
    }

    void DCListViewImpl::SetupHeader(CRect const& rc) const
    {
        struct HeaderText
        {
            int        nWidth;
            TCHAR* pszCaption;
            int          nLen;
        };

        static TCHAR        hdrText0[]{_T("#")};
        static TCHAR        hdrText1[]{_T("TS")};
        static TCHAR        hdrText2[]{_T("Debug")};
        static HeaderText hdrStrings[DCCI_COUNT]{
            { 32, hdrText0, _countof(hdrText0)},
            {128, hdrText1, _countof(hdrText1)},
            {512, hdrText2, _countof(hdrText2)}
        };

        WTL::CHeaderCtrl header{console_.GetHeader()};
        HDITEM          hdrItem{0};
        int              nIndex{0};
        int const          nCCX{rc.Width()};

        hdrStrings[DCCI_Numba].nWidth = nCCX / 16;
        hdrStrings[DCCI_TS].nWidth = nCCX / 8;
        hdrStrings[DCCI_Text].nWidth = nCCX - (hdrStrings[DCCI_Numba].nWidth + hdrStrings[DCCI_TS].nWidth);

        for (auto const& it: hdrStrings) {
            hdrItem.fmt = HDF_LEFT | HDF_STRING;
            hdrItem.cxy = it.nWidth;
            hdrItem.pszText = it.pszCaption;
            hdrItem.cchTextMax = it.nLen;
            hdrItem.mask = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
            header.InsertItem(nIndex, &hdrItem);
            ++nIndex;
        }
    }

    HWND DCListViewImpl::CreateConsole()
    {
        LONG constexpr nRebarCY{22};
        HRESULT           hCode{S_OK};
        ATL::CStringW     sFunc{L"NONE"};
        CRect                rc{};
        GetClientRect(rc);
        toolBox_ = WTL::CFrameWindowImplBase<>::CreateSimpleReBarCtrl(m_hWnd,
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
            RBS_VARHEIGHT | RBS_BANDBORDERS | RBS_AUTOSIZE |
            TBSTYLE_FLAT
        );
        if (toolBox_) {
            CRect rcToolBox{rc};
            rcToolBox.bottom = rc.top + nRebarCY;
            rc.top += rcToolBox.Height();
            toolBox_.MoveWindow(rcToolBox, FALSE);
        }
        else {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"CreateSimpleReBarCtrl failed: 0x%08x %s\r\n", hCode,
                Str::ErrorCode<>::SystemMessage(hCode).GetString());
            DH::TPrintf(LTH_DBG_ASSIST L" %s", sFunc.GetString());
        }
        console_.Create(m_hWnd, rc, nullptr,
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
            LVS_REPORT | LVS_ALIGNLEFT,
            0,
            ID_LOG_CTL
        );
        if (!console_) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"CreateWindowEx('%s')", WTL::CListViewCtrl::GetWndClassName());
            goto reportError;
        }
        console_.SetFont(consoleFont_, FALSE);
        SetupHeader(rc);
        return console_;
    reportError:
        ATL::CStringW sMessage{};
        sMessage.Format(L"%s failed: 0x%08x %s\r\n", sFunc.GetString(), hCode,
            Str::ErrorCode<>::SystemMessage(hCode).GetString());
        if (console_) {
            PutsWide(sMessage.GetString());
        }
        else {
            DH::TPrintf(LTH_DBG_ASSIST L" %s", sMessage.GetString());
        }
        return nullptr;
    }

    void DCListViewImpl::PreWrite()
    {
        //int n = console_.GetWindowTextLength();
        //console_.SetSel(n, n);
    }

    void DCListViewImpl::WriteNarrow(std::string& nrString)
    {
        LVITEMA lvaItem{0};
        lvaItem.mask = LVIF_COLUMNS | LVIF_TEXT;
        lvaItem.cColumns = DCCI_Text;
        lvaItem.cchTextMax = static_cast<int>(nrString.length());
        lvaItem.pszText = nrString.data();
        lvaItem.iItem = -1;
        ::SendMessageA(console_, LVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&lvaItem));
    }

    void DCListViewImpl::WriteWide(std::wstring& wdString)
    {
        LVITEMW lvwItem{0};
        lvwItem.mask = LVIF_COLUMNS | LVIF_TEXT;
        lvwItem.cColumns = DCCI_Text;
        lvwItem.cchTextMax = static_cast<int>(wdString.length());
        lvwItem.pszText = wdString.data();
        lvwItem.iItem = -1;
        ::SendMessageW(console_, LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&lvwItem));
    }

    void DCListViewImpl::PostWrite()
    {
        if (GetParameters().autoScroll) {
            //int const n{console_.GetLineCount() - lastLineCount_};
            //console_.LineScroll(n);
        }
    }

    void DCListViewImpl::OnCommand(UINT, int nID, HWND)
    {
        UNREFERENCED_PARAMETER(nID);
        //if (ID_DC_BN_CLEAN == id) {
        //    Clean();
        //}
        //else if (ID_DC_BN_SAVE == id) {
        //    AskPathAndSave();
        //}
    }

    void DCListViewImpl::Save(char const* filePathName) const
    {
        UNREFERENCED_PARAMETER(filePathName);
        //std::streamsize const len{::GetWindowTextLengthA(console_)};
        //if (len > 0) {
        //    // TODO: save rich text (RTF)!
        //    std::string buffer(static_cast<std::string::size_type>(len)+1, '\0');
        //    int const storedLen{::GetWindowTextA(console_, &buffer[0], static_cast<int>(len))};
        //    if (storedLen > 0) {
        //        std::ofstream output(filePathName);
        //        output << buffer.c_str() << std::flush;
        //    }
        //}
    }

    void DCListViewImpl::OnDestroy()
    {
        if (toolBox_.m_hWnd) {
            toolBox_.DestroyWindow();
        }
        console_.DestroyWindow();
    }
}
