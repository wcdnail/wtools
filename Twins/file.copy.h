#pragma once

#include "file.operation.base.h"

namespace Twins
{
    class CopyFiles: public OperationBase
    {
    private:
        typedef OperationBase Super;

    public:
        virtual ~CopyFiles();
        CopyFiles(std::wstring const& path, std::wstring const& threadName = L"FileCopy:");

        void InitPrompt(PromptDialog& dialog, FileList files) const;

    private:
        virtual void ProcessEntry(Fl::Entry const& source, Fs::path const& destpath, Params& p) const;
        virtual bool OnParseDirectoryStart(Fl::Entry const& source, Fs::path const& destpath, Params& p) const;
    };
}
