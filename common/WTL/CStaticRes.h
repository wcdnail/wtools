#pragma once

#include <atlgdi.h>

struct CStaticRes
{
    WTL::CBitmap  m_bmCross;
    WTL::CIcon  m_icoPicker;

    static CStaticRes const& Instance();

private:
    ~CStaticRes();
    CStaticRes();

    void MakeBitmaps();
};
