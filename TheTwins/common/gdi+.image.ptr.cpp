#include "stdafx.h"
#include "gdi+.image.ptr.h"
#include "windows.resource.custom.h"

namespace Ui
{
    ImagePtr::ImagePtr()
        : image_(NULL)
        , data_()
        , guid_()
    {}

    ImagePtr::ImagePtr(PCTSTR name, PCTSTR type, HMODULE module/* = NULL*/)
        : image_(NULL)
        , data_()
        , guid_()
    {
        Load(name, type, module);
    }

    ImagePtr::ImagePtr(PCWSTR fileName)
        : image_(NULL)
        , data_()
        , guid_()
    {
        Load(fileName);
    }

    Gdiplus::Image *ImagePtr::Get() const
    {
        return image_;
    }

    ImagePtr::~ImagePtr()
    {
        Free();
    }

    void ImagePtr::Free()
    {
        if (NULL != image_)
        {
            delete image_;
            image_ = NULL;
        }
    }

    void ImagePtr::Reset(Gdiplus::Image *image)
    {
        Free();
        image_ = image;
    }

    int ImagePtr::GetWidth() const
    {
        return NULL == image_ ? -1 : (int)image_->GetWidth();
    }

    int ImagePtr::GetHeight() const
    {
        return NULL == image_ ? -1 : (int)image_->GetHeight();
    }
    
    void ImagePtr::Load(PCTSTR name, PCTSTR type, HMODULE module/* = NULL*/)
    {
        Res::Custom resource(name, type, module);
        Res::Data data(resource);
        Gdiplus::Image *temp = Gdiplus::Image::FromStream(data.GetStream());
        Init(temp);
        Reset(temp);
        data_.Swap(data);
    }

    void ImagePtr::Load(PCWSTR fileName)
    {
        Res::Data dummy;
        Gdiplus::Image *temp = Gdiplus::Image::FromFile(fileName);
        Init(temp);
        Reset(temp);
        data_.Swap(dummy);
    }

    void ImagePtr::Init(Gdiplus::Image *image)
    {
        GUIDs guid;

        UINT count = image->GetFrameDimensionsCount();
        if (count > 0)
        {
            guid.reset(new GUID[count]);
            if (Gdiplus::Ok == image->GetFrameDimensionsList(guid.get(), count))
            {
                //for (UINT i=0; i<count; i++)
                //{
                //    wchar_t buffer[40] = {0};
                //    ::StringFromGUID2(guid[i], buffer, _countof(buffer)-1);
                //    UINT fc = image->GetFrameCount(&guid[i]);
                //    ATLTRACE(L"LDIMAG: %3d. `%s` #%d\n", i + 1, buffer, fc);
                //}
            }
        }

        guid_.swap(guid);
    }

    UINT ImagePtr::GetFrameCount() const
    {
        return (NULL == image_ ? 0 : (NULL == guid_.get() ? 0 : image_->GetFrameCount(&guid_[0])));
    }

    bool ImagePtr::SetFrame(int n)
    {
        return NULL == image_ ? false : (NULL == guid_.get() ? false : (Gdiplus::Ok == image_->SelectActiveFrame(&guid_[0], (UINT)n)));
    }

    void ImagePtr::FromDc(HDC sourceDc)
    {
// ##TODO: Implementation"))
    }
}
