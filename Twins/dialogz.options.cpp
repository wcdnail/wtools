#include "stdafx.h"
#include "dialogz.options.h"
#include "twins.state.h"
#include <twins.langs/twins.lang.strings.h>
#include <dh.tracing.h>
#include <dialogz.messagebox.h>
#include <atldlgs.h>

namespace Twins
{
    enum AppConfBarIcons
    {
        GreenRound = 0,
        YellowRound,
    };

    AppConfigDialog::AppConfigDialog(PCWSTR caption, OnApplyCallback const& applyCallback)
        : Super(IDD_APP_OPTIONS, caption)
        , Icons()
        , BoldFont(::CreateFont(-12, 0, 0, 0, FW_BOLD, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, 6, 0, _T("Tahoma")))
        , Categories()
        , RcPage()
        , ApplyCallback(applyCallback)
        , ActivePage(NULL)
        , FileManagerPage()
        , AppearancePage()
    {}

    AppConfigDialog::~AppConfigDialog()
    {}

    bool AppConfigDialog::IsLanguageChaged() const
    {
        return FileManagerPage.IsLanguageChaged();
    }

    BOOL AppConfigDialog::OnInitDialog(HWND, LPARAM param)
    {
        CFontHandle font(BoldFont);

        Categories.Attach(GetDlgItem(IDC_CONFCATEGORY));

        Categories.SetWindowLong(GWL_EXSTYLE, 0x0004 /* TVS_EX_DOUBLEBUFFER */
                                            | 0x0020 /* TVS_EX_AUTOHSCROLL */
                                            | Categories.GetWindowLong(GWL_EXSTYLE));

        if (Icons.Create(18, 18, ILC_COLOR32 | ILC_MASK, 2, 1))
        {
            CBitmap bitmap;
            bitmap.LoadBitmap(IDB_APPCONF_BAR);
            Icons.Add(bitmap, RGB(0, 0, 255));
        }

        Categories.SetImageList(Icons);
        Categories.SetFont(font);

        CStatic area(GetDlgItem(IDC_PPAGEAREA));
        area.GetWindowRect(RcPage);
        ScreenToClient(RcPage);
        area.ShowWindow(SW_HIDE);

        AddButton(L"Ok", ResultOk);
        AddButton(_LS(StrId_Export), ResultExport);
        AddButton(_LS(StrId_Import), ResultImport);
        AddButton(_LS(StrId_Apply), ResultApply);
        AddButton(_LS(StrId_Cancel), ResultCancel);

        SetupCategories();
        DlgResize_Init(false, true);
        return TRUE;
    }

    void AppConfigDialog::ApplyChanges()
    {
        FileManagerPage.OnApplyChanges();

        if (ApplyCallback)
            ApplyCallback(*this);
    }

    bool AppConfigDialog::HaveAnyChangesPendins() const
    {
        return FileManagerPage.IsPendingChanges()
            //|| 
            ;
    }

    template <typename Dialog>
    static bool DoImportExport(bool isExport, Dialog& dlg)
    {
        //CStringW pathname;
        //
        //try
        //{
        //    INT_PTR dr = dlg.DoModal();
        //    pathname = dlg.m_ofn.lpstrFile;
        //
        //    if (IDOK == dr)
        //    {
        //        std::string name = CW2A(pathname).m_psz;
        //
        //        if (isExport)
        //            config.Export(name);
        //        else
        //            config.Import(name);
        //
        //        return true;
        //    }
        //}
        //catch (std::exception const& ex)
        //{
        //    CStringW message;
        //
        //    message.Format(L"%s\r\n`%s`\r\n - %S"
        //        , isExport ? _LS(StrId_Exportfailure) : _LS(StrId_Importfailure)
        //        , pathname.GetString()
        //        , ex.what()
        //        );
        //
        //    CF::UserDialog::Ask(GetActiveWindow(), message, _LS(StrId_Error), MB_ICONSTOP);
        //}

        return false;
    }

    int AppConfigDialog::OnCommandById(int id)
    {
        switch (id)
        {
        case ResultImport: {
            WTL::CFileDialog openDlg(TRUE);
            if (DoImportExport(false, openDlg))
                return Super::OnCommandById(id);
            break;
        }
        case ResultExport: {
            WTL::CFileDialog saveDlg(FALSE);
            DoImportExport(true, saveDlg);
            break;
        }

        case ResultOk:
        case ResultApply: {
            ApplyChanges();
            if (IsLanguageChaged()) {
                EndDialog(ResultRestartRequired);
                return ResultRestartRequired;
            }
            if (ResultApply == id) {
                break;
            }
        }
        case ResultCancel: {
            if (HaveAnyChangesPendins()) {
                WString message = _L(StrId_Configurationhaspendingchanges)
                             + L"\r\n" + _L(StrId_Doyoureallywanttoclosethedialog);

                CStringW caption;
                GetWindowText(caption);

                const auto dr = CF::UserDialog::Ask(m_hWnd, message.c_str(), caption, MB_YESNO | MB_ICONWARNING);
                if (CF::DialogResult::Yes != dr.Code)
                    break;
            }
            return Super::OnCommandById(id);
        }
        }

        return id;
    }

    struct Category;
    void _InsertCategories(CTreeViewCtrl& tree, Category const* categories, size_t count, HTREEITEM root);

    struct Category
    {
        Category(OptionPagePtr page, int icon, WString const& caption, Category const* sub = NULL, size_t subcount = 0)
            : Page(page)
            , Icon(icon)
            , Caption(caption.c_str())
            , Root(NULL)
            , Sub(sub)
            , SubCount(subcount)
        {}

        void Insert(CTreeViewCtrl& tree, HTREEITEM root, HTREEITEM after = NULL) const
        {
            Root = _Insert(tree, root, after);

            if (Sub && (SubCount > 0))
            {
                _InsertCategories(tree, Sub, SubCount, Root);
                tree.Expand(Root, TVE_EXPAND);
            }
        }

        HTREEITEM GetRoot() const { return Root; }

    private:
        OptionPagePtr Page;
        int Icon;
        WString Caption;
        mutable HTREEITEM Root;
        Category const* Sub;
        size_t SubCount;

        HTREEITEM _Insert(CTreeViewCtrl& tree, HTREEITEM rootItem, HTREEITEM insertAfter) const
        {
            TVINSERTSTRUCTW it = {0};

            it.hParent = rootItem;
            it.hInsertAfter = insertAfter;
            it.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
            it.item.pszText = (PWSTR)Caption.c_str();
            it.item.cchTextMax = (int)Caption.length();
            it.item.iImage = Icon;
            it.item.iSelectedImage = Icon;

            HTREEITEM item = tree.InsertItem(&it);
            tree.SetItemData(item, (DWORD_PTR)Page);

            if (Page)
                Page->SetTreeItem(item);

            return item;
        }
    };

    static void _InsertCategories(CTreeViewCtrl& tree, Category const* categories, size_t count, HTREEITEM root)
    {
        for (size_t i=0; i<count; i++)
            categories[i].Insert(tree, root);
    }

    LRESULT AppConfigDialog::OnSelectCategory(LPNMHDR)
    {
        HTREEITEM selected = Categories.GetSelectedItem();
        OptionPagePtr page = selected ? (OptionPagePtr)Categories.GetItemData(selected) : NULL;

        if (ActivePage)
            ActivePage->Hide();

        if (page)
        {
            ActivePage = page;
            page->Show(RcPage);
        }

        return 0;
    }

    void AppConfigDialog::OnSomethingChanged(OptionPage const& page, bool pendingChanges, UINT id, HTREEITEM treeRoot)
    {
        if (treeRoot)
        {
            int icon = pendingChanges ? YellowRound : GreenRound;
            Categories.SetItemImage(treeRoot, icon, icon);
        }
    }

    void AppConfigDialog::SetupCategories()
    {
        OptionPage::SomethingChangedCallback changedCallback = std::bind(&AppConfigDialog::OnSomethingChanged, 
            this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        FileManagerPage.Create(m_hWnd, RcPage);
        AppearancePage.Create(m_hWnd, RcPage);

        FileManagerPage.OnSomethingChangedCallback() = changedCallback;
        AppearancePage.OnSomethingChangedCallback() = changedCallback;

        const Category fileman[] = 
        {
            Category(NULL, GreenRound, _L(StrId_Panels)),
            Category(NULL, GreenRound, _L(StrId_View)),
            Category(NULL, GreenRound, _L(StrId_Utils)),
        };
        
        const Category categories[] = 
        {
            Category(&FileManagerPage, GreenRound, _L(StrId_Filemanager), fileman, _countof(fileman)),
            Category(&AppearancePage, GreenRound, _L(StrId_Appearance)),
            Category(NULL, GreenRound, _L(StrId_Reportsandstatistics))
        };

        Categories.LockWindowUpdate(TRUE);
        _InsertCategories(Categories, categories, _countof(categories), TVI_ROOT);
        Categories.LockWindowUpdate(FALSE);

        Categories.Select(categories[0].GetRoot(), TVGN_CARET);
    }
}
