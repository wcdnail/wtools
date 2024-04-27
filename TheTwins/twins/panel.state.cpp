#include "stdafx.h"
#if 0
#include "panel.state.h"
#include <rect.putinto.h>
#include <rect.gdi+.h>

namespace Twins
{
    PanelState::PanelState()
        : CF::DoubleBuffered(0xffffff, true)
        , animPtr_(NULL)
        , caption_()
        , action_()
        , rcAction_()
        , capFont_(L"Courier New", 10)
        , actFont_(L"Courier New", 10, Gdiplus::FontStyleUnderline)
        , textFormat_()
        , textBrush_(Gdiplus::Color(0xff000000))
        , actBrush_(Gdiplus::Color(0xff0000ff))
        , cursor_(::LoadCursor(NULL, IDC_APPSTARTING))
        , actionCursor_(::LoadCursor(NULL, IDC_HAND))
        , actionCallback_(DummyAction)
        , backgroundImg_()
    {
        textFormat_.SetAlignment(Gdiplus::StringAlignmentCenter);
    }

    PanelState::~PanelState()
    {}

    void PanelState::SetAnimation(Ui::Animation& anim) { animPtr_ = &anim; }
    void PanelState::RemoveAnimation() {  animPtr_ = NULL; }         
    CStringW& PanelState::CaptionText() { return caption_; }
    CStringW& PanelState::ActionText() { return action_; }
    PanelState::ActionCallback& PanelState::Action() { return actionCallback_; }

    int PanelState::OnCreate(LPCREATESTRUCT)
    {
        SetCursor(cursor_);
        return 0;
    }

    void PanelState::OnPaint(CDCHandle)
    {
        WTL::CPaintDC paintDc(m_hWnd);
        CF::BufferedPaint bufferedPaint(paintDc, GetSecondDc(), IsDoubleBuffered(), m_hWnd);
        CDC& curDc = bufferedPaint.GetCurrentDc();
        if (!curDc.m_hDC)
        {
            ::InvalidateRect(m_hWnd, NULL, FALSE);
            return ;
        }

        CRect rc;
        GetClientRect(rc);
        curDc.FillSolidRect(rc, GetBackgroundColor());

        OnPaint(curDc, rc);
    }

    void PanelState::OnPaint(CDC& dc, CRect const& rc)
    {
        Gdiplus::Graphics graphics(dc);
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintSystemDefault);

        if (NULL != backgroundImg_.m_hDC)
        {
            dc.BitBlt(0, 0, rc.Width(), rc.Height(), backgroundImg_, 0, 0, SRCCOPY);
        }

        CRect rcAnim;
        if (NULL != animPtr_)
        {
            Ui::ImagePtr& image = animPtr_->GetImage();
            rcAnim = image.GetCRect();
        }

        Rc::PutInto(rc, rcAnim, Rc::Center);

        if (NULL != animPtr_)
            animPtr_->OnPaint(dc, graphics, rcAnim);

        CRect rcPercent(rc.left, rcAnim.top - 18, rc.right, rcAnim.top - 2);
        CRect rcCancel(rc.left, rcAnim.bottom + 2, rc.right, rcAnim.bottom + 18);
        Gdiplus::RectF rcfPercent;
        Gdiplus::RectF rcfCancel;

        if (Gdiplus::Ok == graphics.MeasureString(caption_, caption_.GetLength(), &capFont_, ToRectF(rcPercent), &textFormat_, &rcfPercent))
            graphics.DrawString(caption_, caption_.GetLength(), &capFont_, rcfPercent, &textFormat_, &textBrush_);

        if (Gdiplus::Ok == graphics.MeasureString(action_, action_.GetLength(), &actFont_, ToRectF(rcCancel), &textFormat_, &rcfCancel))
        {
            graphics.DrawString(action_, action_.GetLength(), &actFont_, rcfCancel, &textFormat_, &actBrush_);
            rcAction_ = FromRectF(rcfCancel);
        }
    }

    void PanelState::Show()
    {
        ShowWindow(SW_SHOW);

        if (NULL != animPtr_)
            animPtr_->Start(m_hWnd);
    }

    void PanelState::Hide()
    {
        if (NULL != animPtr_)
            animPtr_->Stop(m_hWnd);

        ShowWindow(SW_HIDE);
    }

    void PanelState::OnMouseMove(UINT, CPoint point)
    {
        SetCursor(::PtInRect(rcAction_, point) ? actionCursor_ : cursor_);
    }

    void PanelState::OnLButtonDown(UINT, CPoint point)
    {
        if (::PtInRect(rcAction_, point))
        {
            actionCallback_();
        }
    }

    void PanelState::DummyAction()
    {
        ATLTRACE("PNSTAT: %s\n", __FUNCSIG__);
    }

    void PanelState::SetSourceView(HWND sourceHandle)
    {
        CWindow sourceWindow(sourceHandle);
        CClientDC sourceDc(sourceWindow);
        //backgroundImg_.FromDc(sourceDc);

        CRect rc;
        sourceWindow.GetClientRect(rc);

        int cx = rc.Width();
        int cy = rc.Height();

        CDC tempBk;
        if (NULL != tempBk.CreateCompatibleDC(sourceDc))
        {
            CBitmap tempBm;
            if (NULL != tempBm.CreateCompatibleBitmap(tempBk, cx, cy))
            {
                tempBk.SelectBitmap(tempBm);
                tempBk.BitBlt(0, 0, cx, cy, sourceDc, 0, 0, SRCCOPY);
                backgroundImg_.Attach(tempBk);
            }
        }
    }
}
#endif
