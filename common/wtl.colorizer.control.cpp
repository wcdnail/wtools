#include "stdafx.h"
#ifdef _MSC_VER
#  pragma warning(disable: 4820) // 4820: 'WTL::tagCBRPOPUPMENU': '4' bytes padding added after data member 'WTL::tagCBRPOPUPMENU::cbSize'
#  pragma warning(disable: 4365) // 4365: 'argument': conversion from 'unsigned int' to 'int', signed/unsigned mismatch
#endif
#include "wtl.colorizer.control.h"
#include "wtl.colorizer.control.details.h"
#include "wtl.colorizer.helpers.h"
#include "wtl.colorizer.h"

namespace CF::Colorized
{
#pragma region Generic implementation

    template <typename T>
    Control<T>::~Control()
    {
    }

    template <typename T>
    Control<T>::Control(HWND hwnd, Colorizer& master)
        :    Super()
        , m_Master(master)
    {
        Init(hwnd);
    }

    template <typename T>
    void Control<T>::Init(HWND hwnd)
    {
        InitSpec(hwnd);
        this->SubclassWindow(hwnd);
        Super::OnContruct(this->GetWndClassName());
    }

    template <typename T>
    void Control<T>::InitSpec(HWND hwnd)
    {
        UNREFERENCED_ARG(hwnd);
    }

    template <typename T>
    void Control<T>::OnDestroyThiz()
    {
        this->OnDestroy(this->GetWndClassName());
        this->SetMsgHandled(false);
    }

    template <typename T>
    void Control<T>::OnPaint(CDCHandle)
    {
        this->SetMsgHandled(false);
    }

    template <typename T>
    void Control<T>::OnNcPaint(CRgnHandle rgn)
    {
        BorderFlags bt = Details<T>::GetBorderType(*this);
        if (BorderNone != bt) {
            NcPainContext nc(*this, rgn);
            m_Master.DrawControlBorder(nc.Dc.m_hDC, nc.Rect, bt);
        }
    }

    template <typename T>
    BOOL Control<T>::OnEraseBkgnd(CDCHandle dc)
    {
        CRect rc;
        this->GetClientRect(rc);
        m_Master.OnEraseBackground(dc, rc);
        return TRUE;
    }

    template <typename T>
    DWORD Control<T>::OnPrePaint(int id, LPNMCUSTOMDRAW cd)
    {
        UNREFERENCED_ARG(id);
        UNREFERENCED_ARG(cd);
        this->SetMsgHandled(false);
        return CDRF_DODEFAULT;
    }

    template <typename T>
    DWORD Control<T>::OnItemPrePaint(int id, LPNMCUSTOMDRAW cd)
    {
        UNREFERENCED_ARG(id);
        UNREFERENCED_ARG(cd);
        this->SetMsgHandled(false);
        return CDRF_DODEFAULT;
    }

    template <typename T>
    LRESULT Control<T>::OnGetDispinfo(LPNMHDR header)
    {
        UNREFERENCED_ARG(header);
        this->SetMsgHandled(false);
        return 0;
    }

    template <typename T>
    BOOL Control<T>::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
    {
        bool bOld = this->IsMsgHandled();
        BOOL bRet = OnWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
        this->SetMsgHandled(bOld);
        return bRet;
    }

    template <typename T>
    BOOL Control<T>::OnWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
    {
        BOOL bHandled = TRUE;
        if (m_Master.handleWM(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID)) {
            return TRUE;
        }
        switch(dwMsgMapID) {
        case 0:
            MSG_WM_DESTROY(OnDestroyThiz)
            MSG_WM_PAINT(OnPaint)
            MSG_WM_NCPAINT(OnNcPaint)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
            MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
            if (WM_NOTIFY == uMsg) {
                bHandled    = TRUE;
                LPNMHDR hdr = reinterpret_cast<LPNMHDR>(lParam);
                int  idCtrl = static_cast<int>(wParam);
                switch (hdr->code) {
                case NM_CUSTOMDRAW:  //WTL::CCustomDraw<>
                    lResult = OnCustomDraw(idCtrl, hdr, bHandled);
                    break;
                }
                if (bHandled) {
                    return TRUE;
                }
            }
            break;
        case 1:
            REFLECTED_NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW, OnCustomDraw)
        default:
            ATLTRACE(static_cast<int>(ATL::atlTraceWindowing), 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID);
            ATLASSERT(FALSE);
            break;
        }
        return FALSE;
    }

    template <typename T>
    LRESULT Control<T>::OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
    {
        UNREFERENCED_ARG(idCtrl);
        UNREFERENCED_ARG(pnmh);
        bHandled = FALSE;
        return 0;
    }

#if 0
    template <typename T>
    LRESULT Control<T>::OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
    {
        T* pT = static_cast<T*>(this);
        pT->SetMsgHandled(true);
        auto lpNMCustomDraw = reinterpret_cast<LPNMCUSTOMDRAW>(pnmh);
        DWORD dwRet = 0;
        switch (lpNMCustomDraw->dwDrawStage) {
        case CDDS_PREPAINT:
            dwRet = pT->OnPrePaint(idCtrl, lpNMCustomDraw);
            break;
        case CDDS_POSTPAINT:
            dwRet = pT->OnPostPaint(idCtrl, lpNMCustomDraw);
            break;
        case CDDS_PREERASE:
            dwRet = pT->OnPreErase(idCtrl, lpNMCustomDraw);
            break;
        case CDDS_POSTERASE:
            dwRet = pT->OnPostErase(idCtrl, lpNMCustomDraw);
            break;
        case CDDS_ITEMPREPAINT:
            dwRet = pT->OnItemPrePaint(idCtrl, lpNMCustomDraw);
            break;
        case CDDS_ITEMPOSTPAINT:
            dwRet = pT->OnItemPostPaint(idCtrl, lpNMCustomDraw);
            break;
        case CDDS_ITEMPREERASE:
            dwRet = pT->OnItemPreErase(idCtrl, lpNMCustomDraw);
            break;
        case CDDS_ITEMPOSTERASE:
            dwRet = pT->OnItemPostErase(idCtrl, lpNMCustomDraw);
            break;
        case (CDDS_ITEMPREPAINT | CDDS_SUBITEM):
            dwRet = pT->OnSubItemPrePaint(idCtrl, lpNMCustomDraw);
            break;
        default:
            pT->SetMsgHandled(FALSE);
            break;
        }
        bHandled = pT->IsMsgHandled();
        return dwRet;
    }
#endif

    template <typename T>
    CString Control<T>::GetItemText(int index) const
    {
        UNREFERENCED_ARG(index);
        return _T("");
    }

    template <typename T>
    int Control<T>::GetImageIndex(int index) const
    {
        UNREFERENCED_ARG(index);
        return -1;
    }

    template <typename T>
    HIMAGELIST Control<T>::GetImageList() const
    {
        return nullptr;
    }

    template <typename T>
    void Control<T>::OnDrawItem(int id, LPDRAWITEMSTRUCT di)
    {
        const CString&    text = GetItemText(di->itemID);
        const CImageList ilist = GetImageList();
        const int       iindex = GetImageIndex(di->itemID);
        m_Master.DrawItem(di, text, ilist, iindex);
    }

    template <typename T>
    HBRUSH Control<T>::OnCtlColorStatic(CDCHandle dc, HWND hwnd)
    {
        UNREFERENCED_ARG(hwnd);
        m_Master.SetTextColor(dc);
        return m_Master.MyBackBrush[0];
    }

    template <typename T>
    HBRUSH Control<T>::OnCtlColorListBox(CDCHandle dc, HWND hwnd)
    {
        UNREFERENCED_ARG(hwnd);
        m_Master.SetTextColor(dc);
        return m_Master.MyBackBrush[0];
    }

    template <typename T>
    HBRUSH Control<T>::OnCtlColorEdit(CDCHandle dc, HWND hwnd)
    {
        UNREFERENCED_ARG(hwnd);
        m_Master.SetTextColor(dc);
        return m_Master.MyBackBrush[0];
    }

    template <typename T>
    HBRUSH Control<T>::OnCtlColorScrollBar(CDCHandle dc, HWND hwnd)
    {
        UNREFERENCED_ARG(hwnd);
        m_Master.SetTextColor(dc);
        return m_Master.MyBackBrush[0];
    }

#pragma endregion
#pragma region Static specific

    template <>
    void Control<ZStatic>::OnPaint(CDCHandle dc)
    {
        UNREFERENCED_ARG(dc);
        if (Details<WTL::CStatic>::Normal != Details<WTL::CStatic>::GetAppearType(this->m_hWnd)) {
            SetMsgHandled(false);
        }
        else {
            PaintContext pc(this->m_hWnd, this->GetFont());
            CString text;
            if (this->GetWindowText(text)) {
                m_Master.SetTextColor(pc.PaindDC.m_hDC);
                pc.PaindDC.DrawText(text, text.GetLength(), pc.Rect, Details<WTL::CStatic>::GetDrawTextFormat(this->m_hWnd));
            }
        }
    }

#pragma endregion

#pragma region Button specific

    template <>
    void Control<ZButton>::OnPaint(CDCHandle dc)
    {
        UNREFERENCED_ARG(dc);
        Details<WTL::CButton>::AppearType type = Details<WTL::CButton>::GetAppearType(this->m_hWnd);
        if ((Details<WTL::CButton>::Ownerdraw == type) || (Details<WTL::CButton>::UserButton == type)) {
            SetMsgHandled(false);
        }
        else {
            PaintContext pc(this->m_hWnd, this->GetFont());
            const UINT tformat = Details<WTL::CButton>::GetDrawTextFormat(this->m_hWnd);
            const LONG   style = GetWindowLongW(GWL_STYLE); // ##TODO: check mask -> & 0xf00
            const LONG  estyle = GetWindowLongW(GWL_EXSTYLE);
            const UINT   state = GetState();

            CString text;
            this->GetWindowText(text);
            m_Master.SetTextColor(pc.PaindDC.m_hDC);

            switch (type) {
            case Details<WTL::CButton>::Groupbox: {
                m_Master.DrawGroupBox(pc.PaindDC.m_hDC, pc.Rect, text, style);
                break;
            }
            case Details<WTL::CButton>::PushButton:
            case Details<WTL::CButton>::DefPushButton:
            case Details<WTL::CButton>::Flat: {
                m_Master.DrawButton(pc.PaindDC.m_hDC, pc.Rect, text, tformat, state,
                                   Details<WTL::CButton>::Flat == type,
                                   Details<WTL::CButton>::DefPushButton == type);
                break;
            }
            case Details<WTL::CButton>::Checkbox:
            case Details<WTL::CButton>::AutoCheckbox:
            case Details<WTL::CButton>::ThreeState:
            case Details<WTL::CButton>::Auto3State: {
                m_Master.DrawCheckBox(pc.PaindDC.m_hDC, pc.Rect, text, tformat, style, estyle, state);
                break;
            }
            case Details<WTL::CButton>::RadioButton:
            case Details<WTL::CButton>::AutoRadioButton: {
                m_Master.DrawRadioButton(pc.PaindDC.m_hDC, pc.Rect, text, tformat, style, estyle, state);
                break;
            }
            case Details<WTL::CButton>::Pushbox:
            case Details<WTL::CButton>::UserButton:
            case Details<WTL::CButton>::Ownerdraw: {
                ::DebugBreak();
                break;
            }
            }
        }
    }

#pragma endregion

#pragma region ListBox specific

    template <>
    void Control<ZListBox>::InitSpec(HWND hwnd)
    {
        UINT ns = static_cast<UINT>(::GetWindowLong(hwnd, GWL_STYLE));
        ns |= LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT;
        ::SetWindowLong(hwnd, GWL_STYLE, static_cast<LONG>(ns));
    }

    template <>
    CString Control<ZListBox>::GetItemText(int index) const
    {
        CString result;
        this->GetText(index, result);
        return result;
    }

    template <>
    LRESULT Control<ZListBox>::OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
    {
        SetMsgHandled(true);
        OnDrawItem(idCtrl, reinterpret_cast<LPDRAWITEMSTRUCT>(pnmh));
        bHandled = IsMsgHandled();
        return 0;
    }

#pragma endregion

#pragma region ComboBox specific

    template <>
    CString Control<ZComboBox>::GetItemText(int index) const
    {
        CString result;
        this->GetLBText(index, result);
        return result;
    }

    template <> void Control<ZComboBox>::OnNcPaint(CRgnHandle rgn) {}
    template <> BOOL Control<ZComboBox>::OnEraseBkgnd(CDCHandle dc) { return TRUE; }

    template <>
    void Control<ZComboBox>::OnPaint(CDCHandle)
    {
        PaintContext pc(this->m_hWnd, this->GetFont());

        int          iid = GetCurSel();
        CStringW    text = GetItemText(iid);
        CImageList ilist = GetImageList();
        int       iindex = GetImageIndex(iid);

        m_Master.DrawComboFace(pc.PaindDC.m_hDC, pc.Rect, iid, text, ilist, iindex);
    }

    template <>
    HBRUSH Control<ZComboBox>::OnCtlColorListBox(CDCHandle dc, HWND hwnd)
    {
        if (!m_Spec.m_ListBox.m_hWnd) {
            CRect rc;
            this->GetClientRect(rc);
            m_Master.OnEraseBackground(dc, rc);
            m_Spec.m_ListBox.Attach(hwnd);
        }

        m_Master.SetTextColor(dc);
        return m_Master.MyBackBrush[0];
    }

    template <>
    HBRUSH Control<ZComboBox>::OnCtlColorEdit(CDCHandle dc, HWND)
    {
        m_Master.SetTextColor(dc);
        return m_Master.MyBackBrush[0];
    }

    template <>
    LRESULT Control<ZComboBox>::OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
    {
        SetMsgHandled(true);
        OnDrawItem(idCtrl, reinterpret_cast<LPDRAWITEMSTRUCT>(pnmh));
        bHandled = IsMsgHandled();
        return 0;
    }

#pragma endregion

#pragma region ScrollBar specific

    template <>
    BOOL Control<ZScrollBar>::OnEraseBkgnd(CDCHandle)
    {
        this->SetMsgHandled(false);
        return FALSE;
    }

#pragma endregion

#pragma region TreeView specific

    template <>
    void Control<ZTreeViewCtrl>::InitSpec(HWND hwnd)
    {
        ::SetWindowLongW(hwnd, GWL_EXSTYLE, ::GetWindowLongW(hwnd, GWL_EXSTYLE) | 0x0004 /* TVS_EX_DOUBLEBUFFER */);
    }

    template <>
    LRESULT Control<ZTreeViewCtrl>::OnGetDispinfo(LPNMHDR /*header*/)
    {
        //LPNMTVDISPINFO di = (LPNMTVDISPINFO)header;
        // ##TODO: implemenation of CTreeViewCtrl OnGetDispinfo
        return 0;
    }

    template <>
    DWORD Control<ZTreeViewCtrl>::OnPrePaint(int, LPNMCUSTOMDRAW cd) 
    {
        CDCHandle dc(cd->hdc);
        dc.FillSolidRect(&cd->rc, m_Master.MyBackColor);
        return CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW;
    }

    template <>
    DWORD Control<ZTreeViewCtrl>::OnItemPrePaint(int, LPNMCUSTOMDRAW cd) 
    {
        NMTVCUSTOMDRAW* tv = reinterpret_cast<NMTVCUSTOMDRAW*>(cd);
        HTREEITEM     item = reinterpret_cast<HTREEITEM>(cd->dwItemSpec);
        if (this->GetItemState(item, TVIS_SELECTED | TVIS_DROPHILITED) & (TVIS_SELECTED | TVIS_DROPHILITED)) {
            tv->clrText = m_Master.MyHotTextColor;
            tv->clrTextBk = m_Master.MyHotBackColor;
        }
        else {
            tv->clrText = m_Master.MyTextColor;
            tv->clrTextBk = m_Master.MyBackColor;
        }
        return CDRF_DODEFAULT;
    }

#pragma endregion

    template struct Control<ZStatic>;
    template struct Control<ZButton>;
    template struct Control<ZScrollBar>;
    template struct Control<ZComboBox>;
    template struct Control<ZEdit>;
    template struct Control<ZListBox>;
    template struct Control<ZHeaderCtrl>;
    template struct Control<ZLinkCtrl>;
    template struct Control<ZListViewCtrl>;
    template struct Control<ZTreeViewCtrl>;
    template struct Control<ZComboBoxEx>;
    template struct Control<ZTabCtrl>;
    template struct Control<ZIPAddressCtrl>;
    template struct Control<ZPagerCtrl>;
    template struct Control<ZProgressBarCtrl>;
    template struct Control<ZTrackBarCtrl>;
    template struct Control<ZUpDownCtrl>;
    template struct Control<ZDateTimePickerCtrl>;
    template struct Control<ZMonthCalendarCtrl>;
    template struct Control<ZRichEditCtrl>;
}
