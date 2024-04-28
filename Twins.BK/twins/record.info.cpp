#include "stdafx.h"
#if 0
#include "record.info.h"
#include "fs.enum.h"
#include "directory.watch.notify.h"
#include <dh.tracing.h>
#include <boost/regex/mfc.hpp>

namespace Twins
{
    RecordInfo::RecordInfo()
        : name()
        , type()
        , size()
        , date()
        //, misc()
        //, mode()
        , attr(0)
        , creationTime()
        , lastAccessTime()
        , lastWriteTime()
        , pathName()
        , fileSize(0)
        , intType(NotInitialized)
    {
    }

    RecordInfo::RecordInfo(CString const& path, DirectoryNotify const& ninfo)
        : name()
        , type()
        , size()
        , date()
        //, mode()
        , attr(0)
        , creationTime()
        , lastAccessTime()
        , lastWriteTime()
        , pathName()
        , fileSize(0)
        , intType(NotInitialized)
    {
        RecordInfo temp;
        temp.pathName = Fs::LinkFilename(path, ninfo.Filename);
        if (Fs::UpdateEntry(temp))
        {
            Fs::Split(ninfo.Filename, temp.IsDir(), temp.name, temp.type);
            Swap(temp);
            ResetIntType();
        }
    }

    bool RecordInfo::IsValid() const 
    {
        return NotInitialized != intType; 
    }

    RecordInfo::RecordInfo(RecordInfo const& rhs)
        : name(rhs.name)
        , type(rhs.type)
        , size(rhs.size)
        , date(rhs.date)
        //, mode(rhs.mode)
        , attr(rhs.attr)
        , creationTime(rhs.creationTime)
        , lastAccessTime(rhs.lastAccessTime)
        , lastWriteTime(rhs.lastWriteTime)
        , pathName(rhs.pathName)
        , fileSize(rhs.fileSize)
        , intType(rhs.intType)
    {
    }

    RecordInfo& RecordInfo::operator = (RecordInfo const& rhs)
    {
        RecordInfo temp(rhs);
        Swap(temp);
        return *this;
    }

    void RecordInfo::Swap(RecordInfo& rhs)
    {
        std::swap(name, rhs.name);
        std::swap(type, rhs.type);
        std::swap(size, rhs.size);
        std::swap(date, rhs.date);
        //std::swap(mode, rhs.mode);
        std::swap(attr, rhs.attr);
        std::swap(creationTime, rhs.creationTime);
        std::swap(lastAccessTime, rhs.lastAccessTime);
        std::swap(lastWriteTime, rhs.lastWriteTime);
        std::swap(pathName, rhs.pathName);
        std::swap(fileSize, rhs.fileSize);
        std::swap(intType, rhs.intType);
    }

    void RecordInfo::ResetIntType()
    {
        intType = (IsUpDir() ? UpperDirectory : (IsDir() ? Directory : File));
    }

    void RecordInfo::UpdateName(CString const& text)
    {
        if (IsDir())
            name = text;
        
        else
        {
            CString n, e;
            Fs::Split(text, IsDir(), n, e);
            name = n;
            type = e;
        }

        RebuildPathname();
    }

    CString RecordInfo::GetName() const
    {
        return (IsDir() ? name : (type.IsEmpty() ? name : (name + _T(".") + type)));
    }

    void RecordInfo::RebuildPathname()
    {
        if (!IsUpDir())
        {
            CString temp = Fs::GetDir(pathName);
            Fs::LinkWithSeparator(temp, name);
            if (!IsDir() && !type.IsEmpty())
                temp += _T(".") + type;

            pathName = temp;
        }
    }

    bool RecordInfo::Match(CString const& regexp, bool ignoreCase) const
    {
        boost::tregex rex(regexp, (ignoreCase ? boost::tregex::icase : boost::tregex::normal));
        boost::tmatch m;
        if (boost::regex_match(GetName(), m, rex))
            return true;

        return false;
    }

    // TODO: configure witch time must be displayed.
    CTime const& RecordInfo::GetDisplayTime() const { return lastWriteTime; }
    bool RecordInfo::IsDir() const { return 0 != (attr & FILE_ATTRIBUTE_DIRECTORY); }
    bool RecordInfo::IsReparsePoint() const { return 0 != (attr & FILE_ATTRIBUTE_REPARSE_POINT); }
    bool RecordInfo::IsUpDir() const { return IsDir() && (name == _T("..")); }
    bool RecordInfo::IsHidden() const { return 0 != (attr & FILE_ATTRIBUTE_HIDDEN); }

    namespace Less
    {
        static bool ByInternalType(RecordInfo const& L, RecordInfo const& R, bool)
        {
            return L.intType < R.intType;
        }

        static bool ByName(RecordInfo const& L, RecordInfo const& R, bool ascending)
        {
            int cv = L.name.CompareNoCase(R.name);
            return (ascending ? (cv < 0) : (cv > 0));
        }

        static bool ByType(RecordInfo const& L, RecordInfo const& R, bool ascending)
        {
            int cv = L.type.CompareNoCase(R.type);
            return (ascending ? (cv < 0) : (cv > 0));
        }

        static bool BySize(RecordInfo const& L, RecordInfo const& R, bool ascending)
        {
            return (ascending ? (L.fileSize < R.fileSize) : (L.fileSize > R.fileSize));
        }

        static bool ByDate(RecordInfo const& L, RecordInfo const& R, bool ascending)
        {
            return (ascending ? (L.GetDisplayTime() < R.GetDisplayTime()) : (L.GetDisplayTime() > R.GetDisplayTime()));
        }

        template <class P>
        static bool Generic(RecordInfo const& L, RecordInfo const& R, bool ascending, P pred)
        {
            if (R.IsUpDir())
                return false;

            if (L.IsDir() && R.IsDir())
                return ByName(L, R, true);

            if (L.IsDir() || R.IsDir())
                return ByInternalType(L, R, ascending);

            return pred(L, R, ascending);
        }
    }

    bool RecordInfo::LessByName(RecordInfo const& L, RecordInfo const& R, bool ascending)
    {
        return Less::Generic(L, R, ascending, Less::ByName);
    }

    bool RecordInfo::LessByType(RecordInfo const& L, RecordInfo const& R, bool ascending)
    {
        return Less::Generic(L, R, ascending, Less::ByType);
    }

    bool RecordInfo::LessBySize(RecordInfo const& L, RecordInfo const& R, bool ascending)
    {
        return Less::Generic(L, R, ascending, Less::BySize);
    }

    bool RecordInfo::LessByDate(RecordInfo const& L, RecordInfo const& R, bool ascending)
    {
        return Less::Generic(L, R, ascending, Less::ByDate);
    }

    bool RecordInfo::SameName(RecordInfo const& L, CString const& name)
    {
        return L.name == name;
    }

    bool RecordInfo::SameNameAndType(RecordInfo const& L, CString const& name, CString const& type)
    {
        return (L.name == name) && (L.type == type);
    }

    static int FindSubString(CString left, CString right, bool ignoreCase)
    {
        if (ignoreCase)
        {
            left.MakeLower();
            right.MakeLower();
        }

        return left.Find(right);
    }

    bool RecordInfo::NameStartsWith(RecordInfo const& L, CString const& prefix, bool ignoreCase)
    {
        return 0 == FindSubString(L.name, prefix, ignoreCase);
    }
}
#endif // 0
