#include "stdafx.h"
#include "file.move.h"
#include "file.list.h"
#include <twins.lang.strings.h>

namespace Twins
{
    MoveFiles::~MoveFiles() {}

    MoveFiles::MoveFiles(std::wstring const& path)
        : Super(path, L"FileMove:")
    {}

    void InitCopyMovePrompt(PromptDialog& dialog, FileList files, Fs::path const& path, PCWSTR caption, PCWSTR promptFormat);

    void MoveFiles::InitPrompt(PromptDialog& dialog, FileList files) const
    {
        InitCopyMovePrompt(dialog, files, Destpath, _LS(StrId_Movefiles), _LS(StrId_Movesto));
    }

    void MoveFiles::ProcessEntry(Fl::Entry const& source, Fs::path const& destpath, Params& p) const
    {
        BOOL rv = ::MoveFileWithProgressW(source.GetPath().c_str(), destpath.c_str()
            , _ProgressCb, (PVOID)&p
            , 0
            | (p.Flags.Check(SpecFlags::Replace | SpecFlags::ReplaceAll) ? 0 : MOVEFILE_REPLACE_EXISTING)
            // MOVEFILE_COPY_ALLOWED         
            // MOVEFILE_DELAY_UNTIL_REBOOT   
            // MOVEFILE_WRITE_THROUGH        
            // MOVEFILE_CREATE_HARDLINK      
            // MOVEFILE_FAIL_IF_NOT_TRACKABLE
            );

        HRESULT hr = ::GetLastError();
        p.Error.assign(rv ? 0 : (int)hr, p.Error.category());
    }

    void MoveFiles::OnParseDirectoryDone(Fl::Entry const& source, Fs::path const& destpath, Params& p) const
    {
        ::RemoveDirectoryW(source.GetPath().c_str());
    }
}
