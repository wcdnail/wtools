#include "stdafx.h"
#include "icons.dll.h"
#include <shellapi.h>
#include <vector>

using RawIconArray = std::vector<HICON>;

CIconCollectionFile::~CIconCollectionFile()
{
}

CIconCollectionFile::CIconCollectionFile()
    :   m_IconArr{}
    , m_IconArrSm{}
    ,     m_Count{ 0 }
    ,  m_Filename{}
{
}

static CIconCollectionFile::IconArray ConvertToManaged(RawIconArray const& raw, UINT count)
{
    using IconArray = CIconCollectionFile::IconArray;

    if (raw.size() < 1) {
        return {};
    }
    IconArray    result = std::make_unique<CIcon[]>(count);
    HICON const* srcArr = &raw[0];
    CIcon*       dstArr = result.get();

    for (UINT i=0; i<count; i++) {
        if (!srcArr[i]) {
            return {};
        }
        dstArr[i].Attach(srcArr[i]);
    }

    return result;
}

bool CIconCollectionFile::Load(PCWSTR pathname, bool withSmall)
{
    UINT          targetCount = {0};
    UINT          loadedCount = {0};
    HICON*        targetSmPtr = { nullptr };
    RawIconArray     targetSm = {};
    RawIconArray       target = {};
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
    targetCount  = ExtractIconExW(tempPathname.c_str(), -1, nullptr, nullptr, 0);
    if (UINT_MAX == targetCount || targetCount < 1) {
        return false;
    }
    target = RawIconArray(targetCount);
    if (withSmall) {
        targetSm = RawIconArray(targetCount);
        targetSmPtr = &targetSm[0];
    }
    loadedCount = ExtractIconExW(pathname, 0, &target[0], targetSmPtr, targetCount);
    if (loadedCount != targetCount) {
        const auto code = static_cast<HRESULT>(GetLastError());
        if (ERROR_SUCCESS == code) {
            SetLastError(ERROR_INVALID_DATA);
        }
        return false;
    }
    IconArray managedSm;
    if (withSmall) {
        managedSm = ConvertToManaged(target, loadedCount);
        if (!managedSm) {
            SetLastError(ERROR_INVALID_DATA);
            return false;
        }
    }
    IconArray managed = ConvertToManaged(target, loadedCount);
    if (!managed) {
        SetLastError(ERROR_INVALID_DATA);
        return false;
    }
    m_IconArr.swap(managed);
    m_IconArrSm.swap(managedSm);
    m_Filename.swap(tempPathname);
    m_Count = loadedCount;
    return true;
}
