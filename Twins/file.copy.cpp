#include "stdafx.h"
#include "file.copy.h"
#include "file.list.h"
#include "dialogz.file.prompt.h"
#include "dialogz.file.progress.h"
#include <twins.langs/twins.lang.strings.h>
#include <dh.tracing.h>
#include <filesystem>

namespace Twins
{
    CopyFiles::~CopyFiles() {}

    CopyFiles::CopyFiles(std::wstring const& path, std::wstring const& threadName /*= L"FileCopy:"*/)
        : Super(path, threadName)
    {}

    static CString GetFilenameOrCount(FileList files)
    {
        CString rv;
        int count = files.Count();

        if (count == 1)
            rv.AppendFormat(L"`%s`", files[0].GetFilename().c_str());
        else
        {
            int m = count % 10;
            rv.AppendFormat(L"%d %s", count, (m == 1 ? _LS(StrId_File) : ((m > 1 && m < 5) ? _LS(StrId_Filea) : _LS(StrId_Files))));
        }

        return rv;
    }

    void InitCopyMovePrompt(PromptDialog& dialog, FileList files, Fs::path const& path, PCWSTR caption, PCWSTR promptFormat)
    {
        dialog.Setup(caption, true);

        dialog.SetHint(promptFormat, (PCWSTR)GetFilenameOrCount(files));
        dialog.GetHint().ShowWindow(SW_SHOW);

        Fl::Path destpath = path;
        destpath /= (files.Count() == 1 ? files[0].GetFilename().c_str() : L"*");

        dialog.GetPath().SetWindowText(destpath.c_str());
        dialog.GetPath().ShowWindow(SW_SHOW);
    }

    void CopyFiles::InitPrompt(PromptDialog& dialog, FileList files) const
    {
        InitCopyMovePrompt(dialog, files, Destpath, _LS(StrId_Copyfiles), _LS(StrId_Copysto));
    }

    void CopyFiles::ProcessEntry(Fl::Entry const& source, Fs::path const& destpath, Params& p) const
    {
        BOOL rv = ::CopyFileExW(source.GetPath().c_str(), destpath.c_str()
            , _ProgressCb, (PVOID)&p, (LPBOOL)p.Ui.CanceledPtr()
            , (p.Flags.Check(SpecFlags::Replace | SpecFlags::ReplaceAll) ? 0 : COPY_FILE_FAIL_IF_EXISTS)
            | COPY_FILE_ALLOW_DECRYPTED_DESTINATION
// ##TODO: COPY_FILE_COPY_SYMLINK"))
// ##TODO: COPY_FILE_NO_BUFFERING"))
            );

        HRESULT hr = ::GetLastError();
        p.Error.assign(rv ? 0 : (int)hr, p.Error.category());
    }

    bool CopyFiles::OnParseDirectoryStart(Fl::Entry const& source, Fs::path const& destpath, Params& p) const
    {
        Fs::create_directory(destpath, p.Error);
        return !p.Error;
    }
}
