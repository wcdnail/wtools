#pragma once

#include "file.operation.types.h"
#include <atlwin.h>

namespace Twins
{
    class FileAdditionalDialog: CDialogImpl<FileAdditionalDialog>
    {
    private:
        typedef CDialogImpl<FileAdditionalDialog> Super;

    public:
        UINT IDD;

        ~FileAdditionalDialog();
        FileAdditionalDialog(SpecFlags& flags);

        using Super::operator HWND;
        using Super::Create;
        using Super::ShowWindow;
        using Super::GetWindowRect;
        using Super::SetWindowPos;
        using Super::EnableWindow;

    private:
        friend Super;
        CButton Check[9];
        SpecFlags& Flags;

        BOOL OnInitDialog(HWND, LPARAM param);
        void OnDestroy();

        void SetCheck(int id, unsigned mask);
        void GetCheck(int id, unsigned mask);

        BEGIN_MSG_MAP_EX(FileAdditionalDialog)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()
    };
}
