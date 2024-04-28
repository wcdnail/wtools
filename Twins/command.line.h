#pragma once

#include <reflection/ref.std.h>
#include <wtl.combobox.h>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <atltypes.h>
#include <atlwin.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atlframe.h>
#include <atlgdi.h>

namespace Ui
{

    class CommandLine: ATL::CWindowImpl<CommandLine>
                     , boost::noncopyable
    {
    private:
        DECLARE_WND_CLASS_EX(_T("[WCD]CommandLine"), CS_VREDRAW | CS_HREDRAW, COLOR_WINDOW-1)
        
    public:
        typedef ATL::CWindowImpl<CommandLine> Super;
        typedef std::vector<std::wstring> HistoryStore;
        typedef Ref::Container<HistoryStore> RefHistoryStore;
        typedef std::function<bool(CString const&)> EnterCallback;
        typedef std::function<void(WTL::CComboBoxEx& combo)> CancelCallback;

        CancelCallback OnCancel;
        RefHistoryStore History;

        CommandLine(EnterCallback const& onEnter);
        ~CommandLine();

        using Super::ShowWindow;
        using Super::SetFocus;
        using Super::operator HWND;

        bool Create(HWND parent, CRect const& rc, UINT id);
        void SetPath(std::wstring const& text);
        void ShowDropdown();
        void AddText(std::wstring const& text, bool addDirSeparator);

    private:
        friend class Super;
        friend class Resizer;

        enum { IdCombo = 2000, IdPrompt };

        WTL::CStatic Prompt;
        std::mutex PromptMx;
        CAdvComboBox Combo;
        EnterCallback OnEnter;
        WTL::CFont MyFont;
        COLORREF MyTextColor;
        COLORREF MyBackColor;
        CBrush MyBackBrush;

        int OnCreate(LPCREATESTRUCT);
        void OnDestroy();
        void OnSize(UINT type, CSize sz);
        void OnSetFocus(CWindow prev);
        void OnCommand(UINT code, int id, CWindow control);
        LRESULT OnEndEditNotify(LPNMHDR nmh);
        LRESULT OnSelEndCancel(LPNMHDR nmh);
        void ToHistory(CString const& text);
        void LoadHistoryFromCombo();
        BOOL OnEraseBkgnd(CDCHandle dc);
        HBRUSH OnCtlColorStatic(CDCHandle dc, HWND);
        HBRUSH OnCtlColorEdit(CDCHandle dc, HWND);

        BEGIN_MSG_MAP_EX(StatusBar)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_SIZE(OnSize)
            MSG_WM_SETFOCUS(OnSetFocus)
            NOTIFY_HANDLER_EX(IdCombo, CBEN_ENDEDIT, OnEndEditNotify)
            NOTIFY_HANDLER_EX(IdCombo, CBN_SELENDCANCEL, OnSelEndCancel)
            MSG_WM_ERASEBKGND(OnEraseBkgnd)
            MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
            MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
        END_MSG_MAP()
    };
}
