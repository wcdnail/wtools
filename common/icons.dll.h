#pragma once

#include "wcdafx.api.h"
#include <atluser.h>
#include <string>
#include <vector>

using IconArray = std::vector<CIcon>;

class CIconCollectionFile
{
    DELETE_COPY_MOVE_OF(CIconCollectionFile);

public:
    WCDAFX_API ~CIconCollectionFile();
    WCDAFX_API CIconCollectionFile();

    WCDAFX_API bool Load(PCWSTR pathname, bool withSmall);
    std::wstring const& GetFilename() const;
    IconArray const& GetArray() const;
    IconArray const& GetArraySm() const;
    size_t GetCount() const;
    WCDAFX_API WTL::CImageList MakeImageList(bool bigIcons, UINT flags = ILC_MASK | ILC_COLOR32) const;

private:
    IconArray     m_IconArr;
    IconArray   m_IconArrSm;
    std::wstring m_Filename;
};

inline std::wstring const& CIconCollectionFile::GetFilename() const
{
    return m_Filename;
}

inline IconArray const& CIconCollectionFile::GetArray() const
{
    return m_IconArr;
}

inline IconArray const& CIconCollectionFile::GetArraySm() const
{
    return m_IconArrSm;
}


inline size_t CIconCollectionFile::GetCount() const
{
    return m_IconArr.size();
}
