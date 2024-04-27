#pragma once

#if 0
#include "file.operations.types.h"

namespace Twins
{
    class Panel;

    namespace Fo // File operations...
    {
        enum
        {
            Normal     = 0x0000,
            NoUiPrompt = 0x0001,
        };
        
        inline HRESULT CopyFiles(StringArray const& files, Panel& destination, unsigned flags = Normal, HWND parent = NULL) { return S_FALSE; }
        inline HRESULT CopyFiles(Panel& source, Panel& destination, unsigned flags = Normal, HWND parent = NULL) { return S_FALSE; }

        // Obsoletes...
        inline HRESULT PerformMove(Panel& source, Panel& destination, HWND mainFrame = NULL) { return S_FALSE; }
        inline HRESULT PerformDelete(Panel& source, bool toTrash, HWND mainFrame = NULL) { return S_FALSE; }
    }
}
#endif
