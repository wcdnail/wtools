#pragma once

#include "file.operation.base.h"

namespace Twins
{
    class DeleteToTrash: public OperationBase
    {
    private:
        typedef OperationBase Super;

    public:
        virtual ~DeleteToTrash();
        DeleteToTrash();

        void InitPrompt(PromptDialog& dialog, FileList files) const;

    private:
        virtual void OnProcessEntryStart(Fl::Entry const& entry, Fs::path& destpath, Fs::path const& path, Params& p) const;
        virtual void ProcessEntry(Fl::Entry const& source, Fs::path const& destpath, Params& p) const;
        virtual bool OnDirectory(Fl::Entry const& source, Fs::path const& destpath, Params& p) const;
    };
}
