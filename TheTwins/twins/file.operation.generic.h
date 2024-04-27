#pragma once

#include "file.operation.types.h"
#include "dialogz.file.prompt.h"
#include "dialogz.file.progress.h"
#include "panel.h"

namespace Twins
{
    SpecFlags& GetGlobalOperationFlags();

    template <class Operation>
    inline OperationResult PerformOperation(FileList files, Operation const& operation, HWND parent = NULL)
    {
        if (files.Count() < 1)
            return OperationResult(ERROR_BAD_ARGUMENTS, Sy::system_category());

        OperationResult rv(ERROR_CANCELLED, Sy::system_category());
        SpecFlags& flags = GetGlobalOperationFlags();

        PromptDialog prompt(flags);
        int promptRv = prompt.Show(operation, files, parent);

        if (  (promptRv == CommonDialog::ResultOk) 
           || (promptRv == CommonDialog::ResultStart)
           || (promptRv == CommonDialog::ResultStartInIdle)
           )
        {
            FileProgressDialog progress(prompt.GetCaption());
            progress.Start(operation, files, flags, rv, parent);
        }

        return rv;
    }

    template <class Operation>
    inline OperationResult PerformOperation(Panel const& panel, Operation const& operation, HWND parent = NULL)
    {
        OperationResult rv(ERROR_BAD_ARGUMENTS, Sy::system_category());

        Fl::List files;
        if (panel.GetSelection(files))
            rv = PerformOperation(files, operation, parent);

        return rv;
    }
}
