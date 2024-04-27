#pragma once

#include "file.operation.types.h"
#include "dialogz.common.h"
#include "dialogz.file.additional.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace Twins
{
    class PromptDialog: CommonDialog
    {
    private:
        typedef CommonDialog Super;

    public:
        ~PromptDialog();
        PromptDialog(SpecFlags& flags);

        using Super::SetCaption;
        using Super::GetCaption;
        using Super::AddButton;

        template <class Operation>
        int Show(Operation const& operation, FileList files, HWND parent = NULL);

        void Setup(PCWSTR caption, bool additionButton);

        CStatic& GetHint() { return Hint; }
        CEdit& GetPath() { return Path; }
        CStatic& GetList() { return List; }
        CStatic& GetIcon() { return Icon; }

        void SetHint(PCWSTR format, ...);
        void AdjustControlPositions();

    private:
        typedef boost::function<void(void)> InitFunction;
        InitFunction Init;

        CStatic Hint;
        CEdit Path;
        CStatic Icon;
        CStatic List;
        bool Additional;
        CString AdditionalName;
        FileAdditionalDialog RCDialog;

        void ToggleAdditional();
        virtual int OnCommandById(int id);
        BOOL OnInitDialog(HWND, LPARAM param);
        void OnDestroy();
        void OnWindowPosChanged(LPWINDOWPOS pos);

        BEGIN_MSG_MAP_EX(PromptDialog)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
        END_MSG_MAP()
    };

    template <class Operation>
    inline int PromptDialog::Show(Operation const& operation, FileList files, HWND parent /*= NULL*/)
    {
        Init = boost::bind(&Operation::InitPrompt, &operation, boost::ref(*this), boost::cref(files));

        INT_PTR rv = DoModal(parent);
        return (int)rv;
    }
}
