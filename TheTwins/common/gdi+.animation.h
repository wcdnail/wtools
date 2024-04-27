#pragma once

#include "gdi+.image.ptr.h"
#include <boost/noncopyable.hpp>

namespace Ui
{
    class Animation: boost::noncopyable
    {
    private:
        typedef boost::scoped_array<UINT> UIntArray;

    public:
        Animation(UINT start = 0, UINT step = 1, bool loop = true);
        Animation(PCTSTR name, PCTSTR type, UINT start = 0, UINT step = 1, bool loop = true, HMODULE module = NULL);
        Animation(PCWSTR fileName, UINT start = 0, UINT step = 1, bool loop = true);
        ~Animation();

        ImagePtr& GetImage();

        void Load(PCTSTR name, PCTSTR type, HMODULE module = NULL);
        void Load(PCWSTR fileName);

        void Start(HWND owner, UINT defaultFrameDelay = 100);
        void Stop(HWND owner);

        void OnPaint(CDC& dc, CRect const& rc) const;
        void OnPaint(CDC& dc, Gdiplus::Graphics& graphics, CRect const& rc) const;

    private:
        ImagePtr image_;
        UINT frame_;
        UINT step_;
        bool loop_;
        UINT defaultFrameDelay_;
        UIntArray frameDelay_;

        void InitFramesDelay();
        UINT GetFrameDelay(int frame) const;

        void OnChangeFrame(HWND owner);
        UINT InFrameRange(UINT current) const;

        static void CALLBACK ChangeFrame(HWND owner, UINT message, UINT_PTR id, DWORD elapsed);
    };
}
