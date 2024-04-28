#pragma once

#include <reflection/ref.std.h>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_array.hpp>
#include <atlstr.h>

namespace Fl
{
    class Entry;
    class List;
}

namespace Twins
{
    class ViewStatus;

    class SelectHelper: boost::noncopyable
    {
    public:
        typedef int Bit;
        typedef boost::scoped_array<Bit> SelectionSet;
        typedef std::vector<std::wstring> StringVec;
        typedef Ref::Container<StringVec> RefStringVec;

        SelectHelper();
        ~SelectHelper();

        bool IsSelected(int i) const;
        void Reset(size_t count, ViewStatus& status, HWND owner);
        void Clear(size_t count, ViewStatus& status, HWND owner);
        void Set(int i);
        void Unset(int i);
        void Toggle(int i);
        void Toggle(int start, int count);
        void SelectByRegEx(Fl::List const& items, bool mark, ViewStatus& status, HWND parent = NULL);
        int FirstUnselected(int count) const;
        bool GetSelection(Fl::List const& source, Fl::List& files, Fl::Entry const& hot) const;

        static RefStringVec const& GetRegexHistory();
        static std::wstring const& GetCurrentRegex();

    private:
        SelectionSet Selection;
        bool RxFiles;
        bool RxDirs;
        bool IgnoreCase;

        static RefStringVec RxHistory;
        static std::wstring RegExp;

        bool Valid() const;
        static void StoreMaskToHistory(std::wstring const& mask);
        void SelectByRx(Fl::List const& entries, Fl::Entry const& info, int i, std::wstring const& rx, bool mark, ViewStatus& status, bool ignoreCase);
        static std::wstring GetRegexFromUser(HWND parent, bool mark, bool& selectFiles, bool& selectDirs, bool& ignoreCase);
    };
}
