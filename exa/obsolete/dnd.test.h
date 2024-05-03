#ifndef dnd_test_h__
#define dnd_test_h__

#include <vector>
#include <string>

namespace Simple
{
    typedef std::vector<std::wstring> FileList;

    void InitDragdrop(HWND window);
    void DestroyDragdrop(HWND window);
    void GetFileList(FileList& files);
    HRESULT BeginDrag(HWND window, POINT& pt, FileList const& files);
}

#endif // dnd_test_h__
