#pragma once

#include <atlwin.h>
#include <boost/function.hpp>
#include <wtl.colorizer.h>

namespace Twins
{
    class OptionPage;
    typedef OptionPage* OptionPagePtr;

    void LocalizeControl(HWND hwnd);
    void LocalizeControl(HWND owenr, UINT id);

    class OptionPage: ATL::CDialogImpl<OptionPage>
                    , Cf::Colorizer
    {
    private:
        typedef ATL::CDialogImpl<OptionPage> Super;

    public:
        typedef boost::function<void(OptionPage const&, bool, UINT, HTREEITEM)> SomethingChangedCallback;

        virtual void Init() = 0;
        virtual void Destroy();
        virtual bool ApplyChanges() = 0;
        void Hide();
        void Show(CRect const& rc);
        bool IsPendingChanges() const;
        void SetTreeItem(HTREEITEM item);
        SomethingChangedCallback& OnSomethingChangedCallback();
        void OnApplyChanges();

        using Super::operator HWND;
        using Super::Create;

    protected:
        OptionPage(UINT idd);
        virtual ~OptionPage() = 0;

        using Super::m_hWnd;
        using Super::GetDlgItem;

    private:
        friend class Super;

        bool PendingChanges;
        UINT IDD;
        HTREEITEM TreeRoot;
        SomethingChangedCallback OnSomethingChanged;

        BOOL OnInitDialog(HWND, LPARAM);
        void OnDestroy();
        LRESULT OnNotify(int id, LPNMHDR nh);
        void OnCommand(UINT code, int id, HWND);
        void DetectChanges(int code, int id);
        virtual bool HaveChanges() const;

        BEGIN_MSG_MAP_EX(OptionPage)
            CHAIN_MSG_MAP(Cf::Colorizer)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_NOTIFY(OnNotify)
            MSG_WM_COMMAND(OnCommand)
        END_MSG_MAP()
    };

    inline void OptionPage::Hide()
    {
        Super::ShowWindow(SW_HIDE); 
    }

    inline void OptionPage::Show(CRect const& rc) 
    {
        Super::MoveWindow(rc);
        Super::ShowWindow(SW_SHOW);
    }

    inline bool OptionPage::IsPendingChanges() const { return PendingChanges; }
    inline void OptionPage::SetTreeItem(HTREEITEM item) { TreeRoot = item; }
    inline OptionPage::SomethingChangedCallback& OptionPage::OnSomethingChangedCallback() { return OnSomethingChanged; }
}
