#include "stdafx.h"
#include "panel.view.item.h"
#include <string.utils.human.size.h>
#include <rect.putinto.h>
#include <filesystem>
#include <shellapi.h>

namespace Twins
{
    FItem::FItem()
    {}

    FItem::~FItem()
    {}

    FUpDirItem::~FUpDirItem()
    {
    }

    FUpDirItem::FUpDirItem()
        : FItem()
    {
        ::wcsncpy_s(_dummy, L"..", 3);
        FileAttributes = FILE_ATTRIBUTE_DIRECTORY;
        FileNameLength = 2;
        ShortNameLength = 2;
        ::wcsncpy_s(ShortName, L"..", 2);
        FileName[0] = L'.';
        FileName[1] = L'.';
        FileName[2] = 0;
    }

    static CRect NextRect(CRect const& rc, CRect const& prev, int cx)
    {
        CRect result = rc;

        result.left = prev.right + 2;
        result.right = result.left + cx - 3;

        return result;
    }

    static inline void AddFlag(std::wstring& dest, wchar_t const* string, unsigned flag, unsigned mask)
    {
        dest += (0 != (mask & flag)) ? string : L"-";
    }

    std::wstring FileAttributesToString(unsigned attrs)
    {
        std::wstring rv;

#ifdef _WIN32
        AddFlag(rv, L"R", attrs, FILE_ATTRIBUTE_READONLY);
        AddFlag(rv, L"H", attrs, FILE_ATTRIBUTE_HIDDEN);
        AddFlag(rv, L"S", attrs, FILE_ATTRIBUTE_SYSTEM);
        AddFlag(rv, L"D", attrs, FILE_ATTRIBUTE_DIRECTORY);
        AddFlag(rv, L"A", attrs, FILE_ATTRIBUTE_ARCHIVE);
        AddFlag(rv, L"X", attrs, FILE_ATTRIBUTE_DEVICE);
        AddFlag(rv, L"N", attrs, FILE_ATTRIBUTE_NORMAL);
        AddFlag(rv, L"T", attrs, FILE_ATTRIBUTE_TEMPORARY);
        AddFlag(rv, L"S", attrs, FILE_ATTRIBUTE_SPARSE_FILE);
        AddFlag(rv, L"P", attrs, FILE_ATTRIBUTE_REPARSE_POINT);
        AddFlag(rv, L"C", attrs, FILE_ATTRIBUTE_COMPRESSED);
        AddFlag(rv, L"O", attrs, FILE_ATTRIBUTE_OFFLINE);
        AddFlag(rv, L"I", attrs, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
        AddFlag(rv, L"E", attrs, FILE_ATTRIBUTE_ENCRYPTED);
        AddFlag(rv, L"V", attrs, FILE_ATTRIBUTE_VIRTUAL);
#endif

        return rv;
    }

    static COLORREF GetItemTextColor(CTime const& ts, CTime const& curtime)
    {
        CTimeSpan span = curtime - ts;
        LONGLONG left = span.GetDays();

        if (left > 15)
            return 0x00707070;

        else if (left > 8)
            return 0x00808080;

        else if (left > 1)
            return 0x00878787;

        else if (left > 0)
            return 0x00929292;

        return 0x00a0a0a0;
    }

    wchar_t const* FItem::GetExt(size_t& len) const
    {
        len = 0;

        wchar_t const* end = FileName + FileNameLength;
        wchar_t const* pc = end;

        while (pc > FileName)
        {
            if (L'.' == *pc)
            {
                len = (end - ++pc);
                return pc;
            }
            --pc;
        }

        return L"";
    }

    wchar_t const* FItem::GetFilename(size_t& len) const
    {
        size_t l = 0;
        GetExt(l);
        len = FileNameLength - l;
        return FileName;
    }

    void FItem::Draw(CDCHandle dc, CRect const& rcItem
        , bool selected
        , int *columnCx
        , bool const* columnInUse
        , CTime const& curtime
        ) const
    {
        CTime displayTime = GetDisplayTime();

        COLORREF tcolor = GetItemTextColor(displayTime, curtime);
        dc.SetTextColor(tcolor);

        std::filesystem::path path(std::wstring(FileName, FileNameLength));
        std::wstring pathString = path.wstring();
        std::wstring name;
        std::wstring ext;
        std::wstring size;

        bool isSpecial = IsUpperDir();

        CRect rc = rcItem;
        rc.right = rc.left + rcItem.Height();

        CRect rcIcon(0, 0, 16, 16);
        Rc::PutInto(rc, rcIcon, Rc::Center);

        {
            SHFILEINFO info = {};
            ::SHGetFileInfoW(path.c_str(), FileAttributes, &info, sizeof(info)
                , SHGFI_ICON | SHGFI_USEFILEATTRIBUTES 
                | SHGFI_ADDOVERLAYS | SHGFI_SMALLICON 
                | (IsReparsePoint() ? SHGFI_LINKOVERLAY : 0)
                );

            if (NULL != info.hIcon)
            {
                CIcon icon(info.hIcon);
                dc.DrawIconEx(rcIcon.left, rcIcon.top, icon, rcIcon.Width(), rcIcon.Height(), 0, NULL, DI_NORMAL);
            }
        }

        rc = NextRect(rcItem, rc, columnCx[0] - rc.right);

        if (IsDir())
            name = pathString;
        else
        {
            name = path.filename().replace_extension().wstring();
            ext = path.extension().wstring();
            size = Str::HumanSize(GetSize());
        }

        if (name.empty())
        {
            if (!ext.empty())
                name.swap(ext);
            else
                name = pathString;
        }

        dc.DrawText(name.c_str(), (int)name.length(), rc, DT_LEFT);

        if (columnInUse[1])
        {
            rc = NextRect(rcItem, rc, columnCx[1]);
            if (ext.length() > 1)
            {
                std::wstring dispExt = ext.substr(1);
                dc.DrawText(dispExt.c_str(), (int)dispExt.length(), rc, DT_LEFT);
            }
        }

        if (columnInUse[2])
        {
            rc = NextRect(rcItem, rc, columnCx[2]);
            if (!size.empty())
                dc.DrawText(size.c_str(), (int)size.length(), rc, DT_RIGHT);
        }

        if (columnInUse[3])
        {
            rc = NextRect(rcItem, rc, columnCx[3]);
            std::wstring dtime = displayTime.Format(L"%H:%M %d.%m.%Y").GetString();
            dc.DrawText(dtime.c_str(), (int)dtime.length(), rc, DT_RIGHT);
        }

        if (columnInUse[4])
        {
            rc = NextRect(rcItem, rc, columnCx[4]);
            std::wstring attrs = FileAttributesToString(FileAttributes);
            dc.DrawText(attrs.c_str(), (int)attrs.length(), rc, DT_LEFT);
        }

        if (columnInUse[5])
        {
            rc = NextRect(rcItem, rc, columnCx[5]);
            SHFILEINFO info = {};
            DWORD_PTR rv = ::SHGetFileInfoW(path.c_str(), FileAttributes, &info, sizeof(info), SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME);
            std::wstring shellType = info.szTypeName;
            dc.DrawText(shellType.c_str(), (int)shellType.length(), rc, DT_LEFT);
        }
    }
}
