#pragma once

#include "windows.resource.data.h"
#include <boost/noncopyable.hpp>
#include <boost/scoped_array.hpp>
#include <gdiplus.h>
#include <atltypes.h>

namespace Ui
{
    class ImagePtr: boost::noncopyable
    {
    private:
        typedef boost::scoped_array<GUID> GUIDs;

    public:
        ImagePtr();
        ImagePtr(PCTSTR name, PCTSTR type, HMODULE module = NULL);
        ImagePtr(PCWSTR fileName);
        ~ImagePtr();

        Gdiplus::Image *Get() const;

        void Free();
        void Reset(Gdiplus::Image *image);

        void Load(PCTSTR name, PCTSTR type, HMODULE module = NULL);
        void Load(PCWSTR fileName);

        void FromDc(HDC sourceDc);

        int GetWidth() const;
        int GetHeight() const;

        UINT GetFrameCount() const;
        bool SetFrame(int n);

        CRect GetCRect() const;

    private:
        Gdiplus::Image *image_;
        Res::Data data_;
        GUIDs guid_;

        void Init(Gdiplus::Image *temp);
    };

    inline CRect ImagePtr::GetCRect() const
    {
        return CRect(0, 0, GetWidth(), GetHeight());
    }
}
