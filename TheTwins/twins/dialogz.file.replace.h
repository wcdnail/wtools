#pragma once

#include "dialogz.common.h"
#include "file.operation.types.h"
#include <windows.wtl.popup.menus.h>
#include <atlstr.h>

namespace Twins
{
    class FileReplaceDialog: CommonDialog
    {
    private:
        typedef CommonDialog Super;

    public:
        FileReplaceDialog();
        ~FileReplaceDialog();

        /* Returns `SpecFlags` value */
        unsigned Ask(Fl::Entry const& source, Fl::Entry const& dest, SpecFlags flags, HWND parent = NULL);

    private:
        TabBar TopButtons;
        TabBar MidButtons;
        CString Source;
        CString SourceSizeTime;
        CIcon SourceIcon;
        CString Dest;
        CString DestSizeTime;
        CIcon DestIcon;
        unsigned Result;

        static Cf::PopupMenu Popup;

        enum
        {
            ID_BUTTONS1 = CommonDialog::ID_BUTTONS - 3,
            ID_BUTTONS2
        };

        virtual int OnCommandById(int id);
        BOOL OnInitDialog(HWND, LPARAM param);

        BEGIN_MSG_MAP_EX(FileReplaceDialog)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
        END_MSG_MAP()
    };
}
