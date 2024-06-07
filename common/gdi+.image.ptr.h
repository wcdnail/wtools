#pragma once

#include "windows.resource.data.h"
#include <wcdafx.api.h>
#include <gdiplus.h>
#include <atltypes.h>
#include <memory>

namespace Ui
{
    class ImagePtr
    {
    private:
        typedef std::unique_ptr<GUID[]> GUIDs;

    public:
        DELETE_COPY_MOVE_OF(ImagePtr);

        ImagePtr();
        ImagePtr(PCTSTR name, PCTSTR type, HMODULE module = nullptr);
        ImagePtr(PCWSTR fileName);
        ~ImagePtr();

        Gdiplus::Image *Get() const;

        void Free();
        void Reset(Gdiplus::Image *image);

        void Load(PCTSTR name, PCTSTR type, HMODULE module = nullptr);
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
