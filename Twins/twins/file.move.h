#pragma once

#include "file.copy.h"

namespace Twins
{
    class MoveFiles: public CopyFiles
    {
    private:
        typedef CopyFiles Super;

    public:
        virtual ~MoveFiles();
        MoveFiles(std::wstring const& path);

        void InitPrompt(PromptDialog& dialog, FileList files) const;

    private:
        virtual void ProcessEntry(Fl::Entry const& source, Fs::path const& destpath, Params& p) const;
        virtual void OnParseDirectoryDone(Fl::Entry const& source, Fs::path const& destpath, Params& p) const;
    };
}
