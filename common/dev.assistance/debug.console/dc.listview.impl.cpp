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
        DCCI_PID,
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
        static TCHAR        hdrText2[]{_T("PID")};
        static TCHAR        hdrText3[]{_T("Debug")};
        static HeaderText hdrStrings[DCCI_COUNT]{
            { 32, hdrText0, _countof(hdrText0)},
            {128, hdrText1, _countof(hdrText1)},
            { 64, hdrText2, _countof(hdrText1)},
            {512, hdrText3, _countof(hdrText2)}
        };

        WTL::CHeaderCtrl header{console_.GetHeader()};
        HDITEM          hdrItem{0};
        int              nIndex{0};
        int const          nCCX{rc.Width()};

        hdrStrings[DCCI_Numba].nWidth = nCCX / 16;
        hdrStrings[DCCI_TS].nWidth = nCCX / 8;
        hdrStrings[DCCI_PID].nWidth = nCCX / 12;
        hdrStrings[DCCI_Text].nWidth = nCCX - 
            (hdrStrings[DCCI_Numba].nWidth + hdrStrings[DCCI_TS].nWidth + hdrStrings[DCCI_PID].nWidth);

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
            WS_CHILD | WS_VISIBLE |
            LVS_REPORT |
            LVS_NOSORTHEADER,
            LVS_EX_FULLROWSELECT,
            ID_LOG_CTL
        );
        if (!console_) {
            hCode = static_cast<HRESULT>(GetLastError());
            sFunc.Format(L"CreateWindowEx('%s')", WTL::CListViewCtrl::GetWndClassName());
            goto reportError;
        }
        console_.SetView(LV_VIEW_DETAILS);
        console_.SetFont(consoleFont_, FALSE);
        SetupHeader(rc);

        return console_;
    reportError:
        ATL::CStringW sMessage{};
        sMessage.Format(L"%s failed: 0x%08x %s\r\n", sFunc.GetString(), hCode,
            Str::ErrorCode<>::SystemMessage(hCode).GetString());
        if (console_) {
            PutsWide(sMessage.GetString(), dwCurrentPID);
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

    template <typename Char>
    struct ListViewItemTraits
    {};

    template <>
    struct ListViewItemTraits<char>
    {
        using ItemStruct = LVITEMA;
        static int InsertItem(WTL::CListViewCtrl const& ctlConsole, ItemStruct const& lvItem);
        static int SetItem(WTL::CListViewCtrl const& ctlConsole, ItemStruct const& lvItem);
    };

    template <>
    struct ListViewItemTraits<wchar_t>
    {
        using ItemStruct = LVITEMW;
        static int InsertItem(WTL::CListViewCtrl const& ctlConsole, ItemStruct const& lvItem);
        static int SetItem(WTL::CListViewCtrl const& ctlConsole, ItemStruct const& lvItem);
    };

    static void LV_CheckResult(int nRes, PCWSTR pszCaption)
    {
        if (-1 != nRes) {
            return ;
        }
        auto const hCode = static_cast<HRESULT>(GetLastError());
        DH::TPrintf(LTH_DBG_ASSIST L" %s failed: 0x%08x %s\r\n", pszCaption,
            hCode, Str::ErrorCode<>::SystemMessage(hCode).GetString());
    }

    int ListViewItemTraits<char>::InsertItem(WTL::CListViewCtrl const& ctlConsole, ItemStruct const& lvItem)
    {
        auto const nRes = static_cast<int>(::SendMessageA(ctlConsole, LVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&lvItem)));
        LV_CheckResult(nRes, L"LVM_INSERTITEMA");
        return nRes;
    }

    int ListViewItemTraits<wchar_t>::InsertItem(WTL::CListViewCtrl const& ctlConsole, ItemStruct const& lvItem)
    {
        auto const nRes = static_cast<int>(::SendMessageW(ctlConsole, LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&lvItem)));
        LV_CheckResult(nRes, L"LVM_INSERTITEMW");
        return nRes;
    }

    int ListViewItemTraits<char>::SetItem(WTL::CListViewCtrl const& ctlConsole, ItemStruct const& lvItem)
    {
        auto const bRes = static_cast<BOOL>(::SendMessageA(ctlConsole, LVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&lvItem)));
        if (!bRes) {
            LV_CheckResult(-1, L"LVM_SETITEMA");
            return -1;
        }
        return lvItem.iItem;
    }

    int ListViewItemTraits<wchar_t>::SetItem(WTL::CListViewCtrl const& ctlConsole, ItemStruct const& lvItem)
    {
        auto const bRes = static_cast<BOOL>(::SendMessageW(ctlConsole, LVM_SETITEMW, 0, reinterpret_cast<LPARAM>(&lvItem)));
        if (!bRes) {
            LV_CheckResult(-1, L"LVM_SETITEMW");
            return -1;
        }
        return lvItem.iItem;
    }

    template <typename Char>
    static bool InsertLVItem(WTL::CListViewCtrl&  ctlConsole,
                             int                        nPos,
                             std::basic_string<Char>&&  sNum,
                             std::basic_string<Char>&& sTime,
                             std::basic_string<Char>&&  sPID,
                             std::basic_string<Char>&  sText)
    {
        using    StrType = std::basic_string<Char>;
        using ItemStruct = typename ListViewItemTraits<Char>::ItemStruct;

        ItemStruct lvItem{0};
        lvItem.mask = LVIF_TEXT;
        lvItem.cchTextMax = static_cast<int>(sNum.length()) + 1;
        lvItem.pszText = sNum.data();
        lvItem.iItem = nPos;
        lvItem.iSubItem = DCCI_Numba;
        if (-1 == ListViewItemTraits<Char>::InsertItem(ctlConsole, lvItem)) {
            return false;
        }
        lvItem.cchTextMax = static_cast<int>(sTime.length()) + 1;
        lvItem.pszText = sTime.data();
        lvItem.iSubItem = DCCI_TS;
        if (-1 == ListViewItemTraits<Char>::SetItem(ctlConsole, lvItem)) {
            return false;
        }
        lvItem.cchTextMax = static_cast<int>(sPID.length()) + 1;
        lvItem.pszText = sPID.data();
        lvItem.iSubItem = DCCI_PID;
        if (-1 == ListViewItemTraits<Char>::SetItem(ctlConsole, lvItem)) {
            return false;
        }
        lvItem.cchTextMax = static_cast<int>(sText.length()) + 1;
        lvItem.pszText = sText.data();
        lvItem.iSubItem = DCCI_Text;
        if (-1 == ListViewItemTraits<Char>::SetItem(ctlConsole, lvItem)) {
            return false;
        }
        return true;
    }

    void DCListViewImpl::WriteNarrow(std::string& nrString, double dTs, DWORD dwPID)
    {
        int const nPos{console_.GetItemCount()};
        InsertLVItem<char>(console_, nPos, std::to_string(nPos), std::to_string(dTs), std::to_string(dwPID), nrString);
    }

    void DCListViewImpl::WriteWide(std::wstring& wdString, double dTs, DWORD dwPID)
    {
        int const   nPos{console_.GetItemCount()};
        InsertLVItem<wchar_t>(console_, nPos, std::to_wstring(nPos), std::to_wstring(dTs), std::to_wstring(dwPID), wdString);
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
