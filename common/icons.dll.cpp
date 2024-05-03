#include "stdafx.h"
#include "icons.dll.h"
#include <shellapi.h>
#include <vector>

CIconCollectionFile::~CIconCollectionFile()
{
}

CIconCollectionFile::CIconCollectionFile()
    :   m_IconArr{}
    , m_IconArrSm{}
    ,  m_Filename{}
{
}

bool CIconCollectionFile::Load(PCWSTR pathname, bool withSmall)
{
    enum ExtractStep: int
    {
        Extract_GetCount = -1,
        Extract_StartWith = 0,
    };
    UINT          targetCount = { 0 };
    UINT          loadedCount = { 0 };
    HICON*        targetSmPtr = { nullptr };
    IconArray        targetSm = {};
    IconArray          target = {};
    std::wstring tempPathname = {};
    if (!pathname) {
        SetLastError(ERROR_PATH_NOT_FOUND);
        return false;
    }
    {
        CStringW tempStr = pathname;
        int       bufLen = tempStr.GetLength() + MAX_PATH;
        DWORD   envSubst = DoEnvironmentSubstW(tempStr.GetBufferSetLength(bufLen), bufLen);
        DWORD   substLen = HIWORD(envSubst);
        DWORD    pathLen = LOWORD(envSubst);
        if (pathLen < 1) {
            return false;
        }
        tempStr.ReleaseBufferSetLength(pathLen);
        tempPathname = std::wstring{ tempStr.GetString(), static_cast<size_t>(tempStr.GetLength()) };
    }
    targetCount  = ExtractIconExW(tempPathname.c_str(), Extract_GetCount, nullptr, nullptr, 0);
    if (UINT_MAX == targetCount || targetCount < 1) {
        return false;
    }
    target = IconArray(targetCount);
    if (withSmall) {
        targetSm = IconArray(targetCount);
        targetSmPtr = reinterpret_cast<HICON*>(&targetSm[0]);
    }
    loadedCount = ExtractIconExW(pathname, Extract_StartWith, reinterpret_cast<HICON*>(&target[0]), targetSmPtr, targetCount);
    if (loadedCount != targetCount) {
        const auto code = static_cast<HRESULT>(GetLastError());
        if (ERROR_SUCCESS == code) {
            SetLastError(ERROR_INVALID_DATA);
        }
        return false;
    }
    m_IconArr.swap(target);
    m_IconArrSm.swap(targetSm);
    m_Filename.swap(tempPathname);
    return true;
}

WTL::CImageList CIconCollectionFile::MakeImageList(bool bigIcons, UINT flags /*= ILC_MASK | ILC_COLOR32*/) const
{
    IconArray const& src = bigIcons ? GetArray() : GetArraySm();
    if (src.size() < 1) {
        return {};
    }
    int icoCx = 16;
    int icoCy = 16;
    if (bigIcons) {
        icoCx = 32;
        icoCy = 32;
    }
    WTL::CImageList result;
    if (!result.Create(icoCx, icoCy, flags, (int)src.size(), 0)) {
        return {};
    }
    for (const auto it: src) {
        if (-1 == result.AddIcon(it.m_hIcon)) {
            return {};
        }
    }
    int imListCount = result.GetImageCount();
    int     vecSize = (int)src.size();
    ATLASSUME(imListCount == vecSize);
    return { result.Detach() };
}
