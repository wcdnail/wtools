#pragma once

#include "wcdafx.api.h"
#include <atluser.h>
#include <string>
#include <memory>

class CIconCollectionFile
{
    DELETE_COPY_MOVE_OF(CIconCollectionFile);

public:
    using IconArray = std::unique_ptr<CIcon[]>;

    WCDAFX_API ~CIconCollectionFile();
    WCDAFX_API CIconCollectionFile();

    WCDAFX_API bool Load(PCWSTR pathname, bool withSmall);
    std::wstring const& GetFilename() const;

private:
    IconArray     m_IconArr;
    IconArray   m_IconArrSm;
    UINT            m_Count;
    std::wstring m_Filename;
};

inline std::wstring const& CIconCollectionFile::GetFilename() const
{
    return m_Filename;
}
