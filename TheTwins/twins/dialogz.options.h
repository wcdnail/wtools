#pragma once

#include "options.all.pages.h"
#include "dialogz.common.h"
#include <atlddx.h>
#include <atlctrls.h>
#include <atltypes.h>

namespace Twins
{
    class AppConfigDialog: CommonDialog
                         , CDialogResize<AppConfigDialog>
    {
    private:
        typedef CommonDialog Super;
        typedef CDialogResize<AppConfigDialog> Resizer;
        typedef std::function<void(AppConfigDialog&)> OnApplyCallback;

    public:
        enum
        {
            ResultExport = ResultLast,
            ResultImport,
            ResultApply,
            ResultRestartRequired,
        };

        AppConfigDialog(PCWSTR caption, OnApplyCallback const& applyCallback);
        ~AppConfigDialog();

        using Super::DoModal;

        bool IsLanguageChaged() const;

    private:
        friend class WinDdx;
        friend class Resizer;
                
        CImageList Icons;
        CFont BoldFont;
        CTreeViewCtrl Categories;
        CRect RcPage;
        OnApplyCallback ApplyCallback;
        OptionPagePtr ActivePage;
        FileManagerOptions FileManagerPage;
        AppearanceOptions AppearancePage;

        virtual int OnCommandById(int id);
        BOOL OnInitDialog(HWND, LPARAM param);
        void SetupCategories();
        LRESULT OnSelectCategory(LPNMHDR pnmh);
        bool HaveAnyChangesPendins() const;
        void OnSomethingChanged(OptionPage const& page, bool pendingChanges, UINT id, HTREEITEM treeRoot);
        void ApplyChanges();

        BEGIN_MSG_MAP_EX(AppConfigDialog)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
            NOTIFY_HANDLER_EX(IDC_CONFCATEGORY, TVN_SELCHANGED, OnSelectCategory)
            CHAIN_MSG_MAP(Resizer)
        END_MSG_MAP()

        BEGIN_DLGRESIZE_MAP(Resizer)
            DLGRESIZE_CONTROL(ID_BUTTONS, DLSZ_MOVE_Y | DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(IDC_CONFCATEGORY, DLSZ_SIZE_Y)
            DLGRESIZE_CONTROL(IDC_PPAGEAREA, DLSZ_SIZE_Y | DLSZ_SIZE_X)
        END_DLGRESIZE_MAP()
    };
}
