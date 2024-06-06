#include "stdafx.h"
#include "gdi+.animation.h"
#include "rect.gdi+.h"

namespace Ui
{
    Animation::Animation(UINT start/* = 0*/, UINT step/* = 1*/, bool loop/* = true*/)
        : image_()
        , frame_(start)
        , step_(step)
        , loop_(loop)
        , defaultFrameDelay_(100)
        , frameDelay_()
    {}

    Animation::Animation(PCTSTR name, PCTSTR type, UINT start/* = 0*/, UINT step/* = 1*/, bool loop/* = true*/, HMODULE module/* = nullptr*/)
        : image_(name, type, module)
        , frame_(InFrameRange(start))
        , step_(step)
        , loop_(loop)
        , defaultFrameDelay_(100)
        , frameDelay_()
    {
        InitFramesDelay();
        image_.SetFrame(frame_);
    }

    Animation::Animation(PCWSTR fileName, UINT start/* = 0*/, UINT step/* = 1*/, bool loop/* = true*/) 
        : image_(fileName)
        , frame_(InFrameRange(start))
        , step_(step)
        , loop_(loop)
        , defaultFrameDelay_(100)
        , frameDelay_()
    {
        InitFramesDelay();
        image_.SetFrame(frame_);
    }

    Animation::~Animation() 
    {}

    ImagePtr& Animation::GetImage()
    {
        return image_;
    }

    void Animation::Load(PCTSTR name, PCTSTR type, HMODULE module/* = nullptr*/)
    {
        image_.Load(name, type, module);
        InitFramesDelay();
    }

    void Animation::Load(PCWSTR fileName)
    {
        image_.Load(fileName);
        InitFramesDelay();
    }

    void Animation::Start(HWND owner, UINT defaultFrameDelay/* = 100*/)
    {
        defaultFrameDelay_ = defaultFrameDelay;
        ::InvalidateRect(owner, nullptr, FALSE);
        ::SetTimer(owner, (UINT_PTR)this, GetFrameDelay(frame_), ChangeFrame);
    }

    void Animation::Stop(HWND owner)
    {
        ::KillTimer(owner, (UINT_PTR)this);
    }

    void CALLBACK Animation::ChangeFrame(HWND owner, UINT message, UINT_PTR id, DWORD elapsed)
    {
        Animation* self = (Animation*)id;
        if (nullptr != self)
            self->OnChangeFrame(owner);
    }

    UINT Animation::InFrameRange(UINT current) const
    {
        UINT frameCount = image_.GetFrameCount();

        if (current < 0)
            current = loop_ ? frameCount - 1: 0;

        if (current > frameCount - 1)
            current = (loop_ ? 0 : frameCount - 1);

        return current;
    }

    void Animation::OnChangeFrame(HWND owner)
    {
        Stop(owner);

        image_.SetFrame(frame_);
        frame_ = InFrameRange(frame_ + step_);

        Start(owner, defaultFrameDelay_);
    }

    void Animation::InitFramesDelay()
    {
        Gdiplus::Image *image = image_.Get();
        if (nullptr == image) {
            return ;
        }
        UINT frameCount = image_.GetFrameCount();
        if (frameCount < 1) {
            return ;
        }
        UIntArray tempFrameDelay(new UINT[frameCount]);
        for (UINT i=0; i<frameCount; i++) {
            tempFrameDelay[i] = defaultFrameDelay_;
        }
        UINT len = image->GetPropertyItemSize(PropertyTagFrameDelay);
        if (len > 0) {
            Gdiplus::PropertyItem *delay = (Gdiplus::PropertyItem*)::calloc(1, len);
            if (nullptr != delay) {
                if (Gdiplus::Ok == image->GetPropertyItem(PropertyTagFrameDelay, len, delay)) {
                    for (UINT i=0; i<frameCount; i++) {
                        tempFrameDelay[i] = ((UINT*)delay[0].value)[i] * 10;
                    }
                }
                free((void*)delay);
            }
        }

        frameDelay_.swap(tempFrameDelay);
    }

    UINT Animation::GetFrameDelay(int frame) const
    {
        return nullptr != frameDelay_.get() ? frameDelay_[frame] : defaultFrameDelay_;
    }

    void Animation::OnPaint(CDC& dc, CRect const& rc) const
    {
        Gdiplus::Graphics graphics(dc);
        OnPaint(dc, graphics, rc);
    }

    void Animation::OnPaint(CDC& dc, Gdiplus::Graphics& graphics, CRect const& rc) const
    {
        graphics.DrawImage(image_.Get(), ToRect(rc));
    }
}
