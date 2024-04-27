#pragma once

#include <vector>
#include <atltime.h>

namespace Twins
{
    extern std::wstring FileAttributesToString(unsigned attrs);

    // WARN: Don't add any data to this!
    struct FItem: public FILE_ID_BOTH_DIR_INFO
    {
        FItem();
        ~FItem();
         
        bool IsHidden() const { return 0 != (FILE_ATTRIBUTE_HIDDEN & FileAttributes); }
        bool IsDir() const { return 0 != (FILE_ATTRIBUTE_DIRECTORY & FileAttributes); }
        bool IsReparsePoint() const { return 0 != (FILE_ATTRIBUTE_REPARSE_POINT & FileAttributes); }
        bool IsCurrentDir() const;
        bool IsUpperDir() const;

        wchar_t const* GetExt(size_t& len) const;
        wchar_t const* GetFilename(size_t& len) const;
        std::wstring GetFilename() const;
        unsigned __int64 GetSize() const { return EndOfFile.QuadPart; }
        CTime GetDisplayTime() const { return CTime(CFileTime(LastAccessTime.QuadPart)); }

        void Draw(CDCHandle dc, CRect const& rc
            , bool selected
            , int *columnCx
            , bool const* columnInUse
            , CTime const& curtime
            ) const;
    };

    struct FUpDirItem: public FItem
    {
        FUpDirItem();

    private:
        wchar_t _dummy[2];
    };


    inline bool FItem::IsCurrentDir() const
    {
        return IsDir() 
            && ((1 == FileNameLength)
            &&  (L'.' == *(FileName))
            ); 
    }

    inline bool FItem::IsUpperDir() const
    {
        return IsDir() 
            && ((2 == FileNameLength)
            &&  (L'.' == *(FileName))
            &&  (L'.' == *(FileName + 1))
            ); 
    }

    inline std::wstring FItem::GetFilename() const { return std::wstring(FileName, FileNameLength); }

    typedef std::vector<FItem*> FItemVector;
}
