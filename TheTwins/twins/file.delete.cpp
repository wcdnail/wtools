#include "stdafx.h"
#include "file.delete.h"
#include "dialogz.file.prompt.h"
#include "dialogz.file.progress.h"
#include "file.list.h"
#include <twins.lang.strings.h>
#include <dh.tracing.h>

namespace Twins
{
    DeleteToTrash::~DeleteToTrash()
    {}

    DeleteToTrash::DeleteToTrash()
        : Super(L"", L"FileTrsh:")
    {}

    void DeleteToTrash::InitPrompt(PromptDialog& dialog, FileList files) const
    {
        dialog.Setup(_LS(StrId_Deletingfiles), false);

        dialog.GetPath().ShowWindow(SW_HIDE);

        dialog.GetHint().SetWindowText(_LS(StrId_Reallywanttodeletethefilestothetrash));
        dialog.GetHint().ShowWindow(SW_SHOW);

        dialog.GetList().SetWindowText(files.ToString(L"\r\n", 6).c_str());
        dialog.GetList().ShowWindow(SW_SHOW);

        CIcon icon(::LoadIcon(NULL, IDI_EXCLAMATION));
        ::SendMessage(dialog.GetIcon(), STM_SETICON, (WPARAM)icon.m_hIcon, 0);
        dialog.GetIcon().ShowWindow(SW_SHOW);
    }

    void DeleteToTrash::OnProcessEntryStart(Fl::Entry const& entry, Fs::path& destpath, Fs::path const& path, Params& p) const
    {
        p.Ui.SetSource(destpath.native());
        p.Ui.SetDest(entry.GetPath());
    }

    static void MoveToTrash(std::wstring const& pathname, OperationBase::Params& p)
    {
        CString zzBuffer(L'\0', (int)pathname.length() + 2);
        zzBuffer.SetString(pathname.c_str());

        SHFILEOPSTRUCTW opfo = {0};

        opfo.wFunc  = FO_DELETE;
        opfo.pFrom  = zzBuffer;
        opfo.fFlags = FOF_NOCONFIRMATION 
                    | FOF_NOERRORUI 
                    | FOF_SILENT 
                    | FOF_ALLOWUNDO;

        HRESULT hr = ::SHFileOperationW(&opfo);
        p.Error.assign((int)hr, p.Error.category());
    }

    void DeleteToTrash::ProcessEntry(Fl::Entry const& source, Fs::path const& destpath, Params& p) const
    {
        MoveToTrash(source.GetPath(), p);
        UpdateEntryProgress(source.GetSize(), p);
    }

    bool DeleteToTrash::OnDirectory(Fl::Entry const& source, Fs::path const& destpath, Params& p) const
    {
        ProcessEntry(source, destpath, p);
        return !p.Error;
    }
}
