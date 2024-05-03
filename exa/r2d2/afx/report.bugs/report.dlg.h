#ifndef aw_report_bug_dlg_h__
#define aw_report_bug_dlg_h__

#include "../use.wtl.h"
#include "../report.bugs.h"
#include "../wtl.anchors.h"
#include "resource.h"

class CReportBugDlg: public CDialogImpl<CReportBugDlg>
{
public:
	enum { IDD = IDD_BUGREPORT };

	CReportBugDlg(BugReportBase const& info, char const* callstack = NULL)
        : bugInfo_(info)
        , callstack_(callstack)
    {}

	~CReportBugDlg() {}

	BEGIN_MSG_MAP_EX(CReportBugDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_SIZE(anchors_.OnResize)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_DRAWITEM(OnDrawItem)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBk)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_TIMER(OnTimer)
		COMMAND_ID_HANDLER(IDCANCEL, OnClose)
        COMMAND_ID_HANDLER_EX(IDC_DESCRIBEBUG, OnDescribe)
	END_MSG_MAP()

private:
    BugReportBase const& bugInfo_;
    char const* callstack_;
	CFont capFont_;
    CAnchorArray<3> anchors_;
    CDC backDc_;

    enum {
        EnlargeYAmount = 120
    ,   EnlargeYStep = 14
    };
    
	BOOL OnInitDialog(HWND, LPARAM) 
	{
        PlaySound(MAKEINTRESOURCE(IDR_WAVE1), ModuleHelper::GetResourceInstance(), SND_ASYNC | SND_RESOURCE);

        SetWindowText(_T("UnI bug reporter"));

        // set bug description text
        CEdit bugDesc(GetDlgItem(IDC_BUGINFO));
        try
        {
            std::string message(bugInfo_.GetMessage());
            message += "\r\n\r\n";
            message += callstack_;
            ::SetWindowTextA(bugDesc, message.c_str());
        }
        catch(...)
        {
            ::SetWindowTextA(bugDesc, bugInfo_.GetMessage());
        }

        CIcon icon;
        icon.LoadIcon(IDI_ICON1);
        SetIcon(icon);
        SetIcon(icon, FALSE);

        // anchor controls
        anchors_.LinkWith(*this);
        anchors_.SetAnchor(GetDlgItem(IDCANCEL));
        anchors_.SetAnchor(GetDlgItem(IDC_SENDREPORT));
        anchors_.SetAnchor(GetDlgItem(IDC_BUGINFO), Anchors::SizeY | Anchors::SizeX);

        // creating a background
        CBitmapHandle logoBmp(CStatic(GetDlgItem(IDC_LOGO)).GetBitmap());
        CSize logoSz;
        logoBmp.GetSize(logoSz);

        CRect rc;
        GetClientRect(&rc);
        rc.bottom += EnlargeYAmount + EnlargeYStep*2;
        
        CClientDC cliDc(*this);
        CDC logoDc;
        logoDc.CreateCompatibleDC(cliDc);
        logoDc.SelectBitmap(logoBmp);

        backDc_.CreateCompatibleDC(cliDc);
        CBitmapHandle backBmp;
        backBmp.CreateCompatibleBitmap(cliDc, rc.Width(), rc.Height());
        backDc_.SelectBitmap(backBmp);

        backDc_.StretchBlt(0, 0, rc.right, logoSz.cy, logoDc, 0, 0, 1, logoSz.cy, SRCCOPY);
        backDc_.StretchBlt(0, logoSz.cy, logoSz.cx, rc.bottom-logoSz.cy, logoDc, 0, logoSz.cy-1, logoSz.cx-1, 1, SRCCOPY);
        backDc_.StretchBlt(logoSz.cx, logoSz.cy, rc.right-logoSz.cx, rc.bottom-logoSz.cy, logoDc, logoSz.cx-1, logoSz.cy-1, 1, 1, SRCCOPY);

		return TRUE; 
	}

	void OnLButtonDown(UINT, CPoint) {
        PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0); 
    }

	LRESULT OnClose(WORD, WORD id, HWND, BOOL&) {
        Close(id); 
        return 0; 
    }

	void Close(int code) { EndDialog(code); }
	BOOL OnEraseBk(CDCHandle) { return TRUE; }

	HBRUSH OnCtlColorStatic(CDCHandle dc, CStatic wndStatic) 
	{
        HBRUSH rv = (HBRUSH)GetStockObject(NULL_BRUSH);
        dc.SetBkMode(TRANSPARENT);

        switch (wndStatic.GetDlgCtrlID())
		{
        case IDC_CAPTION:
			if (!capFont_)
			{
				LOGFONT lf;
				GetObject(dc.GetCurrentFont(), sizeof(LOGFONT), &lf);
				lf.lfHeight = -24;
				lf.lfWeight = FW_SEMIBOLD;
				capFont_ = CreateFontIndirect(&lf);
			}
			dc.SelectFont(capFont_);
            break;

        case IDC_BUGINFO:
            dc.SetTextColor(0xffffff);
            dc.SetBkColor(0);
            dc.SetBkMode(OPAQUE);
            rv = (HBRUSH)GetStockObject(BLACK_BRUSH);
            break;
		}

        //CRect rc;
        //wndStatic.GetWindowRect(&rc);
        //ScreenToClient(&rc);
        //dc.BitBlt(0, 0, rc.Width(), rc.Height(), backDc_, rc.left, rc.top, SRCCOPY);

		return rv;
	}

	void OnPaint(CDCHandle)
	{
		CPaintDC dc(*this);
        CRect rc;
        GetClientRect(&rc);
        dc.BitBlt(0, 0, rc.Width(), rc.Height(), backDc_, rc.left, rc.top, SRCCOPY);
	}
	
	void OnDrawItem(int id, PDRAWITEMSTRUCT dis) 
	{
		if (ODT_BUTTON == dis->CtlType) 
		{
            COLORREF textColor = 0;
            int edgeBrushIndex = BLACK_BRUSH;
            if (ODS_DISABLED & dis->itemState) {
                textColor = 0x7f7f7f;
                edgeBrushIndex = GRAY_BRUSH;
            }

			CDCHandle dc(dis->hDC);
			CRect rc(dis->rcItem);
			rc.NormalizeRect();

			dc.FillSolidRect(&dis->rcItem, backDc_.GetPixel(0, 0));
			dc.FrameRect(&dis->rcItem, (HBRUSH)GetStockObject(edgeBrushIndex));

			rc.DeflateRect(2, 2);

			CButton btn(GetDlgItem(id));
			TCHAR text[256] = {0};
			dc.SetBkMode(TRANSPARENT);
            dc.SetTextColor(textColor);
			dc.DrawText(text, btn.GetWindowText(text, _countof(text)-1), rc, DT_VCENTER | DT_SINGLELINE | DT_CENTER);

			if (dis->itemState & ODS_FOCUS)
				dc.DrawFocusRect(rc);
		}
	}

    enum { TIMER_ENLARGE_WINDOW = 1 };
    double enlargeStep_;
    LONG usualHeight_; 

    void OnTimer(UINT_PTR id) 
    {
        if (TIMER_ENLARGE_WINDOW == id)
        {
            CRect rc;
            GetWindowRect(rc);

            if ((rc.Height()-usualHeight_) > EnlargeYAmount) 
                KillTimer(id);
            else
            {
                double y = rc.top - enlargeStep_;
                rc.top = (LONG)y;

                double h = rc.bottom + enlargeStep_;
                rc.bottom = (LONG)h;

                if (enlargeStep_ < 2) enlargeStep_ = 2;
                else enlargeStep_ -= 0.5;

                MoveWindow(rc, TRUE);
            }
        }
    }

    void OnDescribe(UINT, int, CWindow wndCtl)
    {
        wndCtl.EnableWindow(FALSE);
        wndCtl.ShowWindow(SW_HIDE);

        GetDlgItem(IDC_SENDREPORT).ShowWindow(SW_SHOW);
        GetDlgItem(IDC_BUGINFO).ShowWindow(SW_SHOW);
        GetDlgItem(IDCANCEL).SetFocus();

        CRect rc;
        GetWindowRect(&rc);
        usualHeight_ = rc.Height();

        SetTimer(TIMER_ENLARGE_WINDOW, 5);
        enlargeStep_ = EnlargeYStep;
    }
};

#endif // aw_report_bug_dlg_h__
