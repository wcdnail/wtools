#include "stdafx.h"
#if 0
#include "ut.wtl.window.h"
#include <rect.putinto.h>
#include <rect.aspect.ratio.h>
#include <string.utils.error.code.h>
#include <windows.uses.gdi+.h>
#include <gdi+.image.ptr.h>
#include <gdi+.animation.h>
#include <boost/bind.hpp>
#include <memory>
#include "res/resource.h"

namespace Ut
{
    struct Tests
    {
        enum
        {
            BallApng,
            BallSvg,
            WithoutAnimation,
            Felix,
            Recycler,
            Laptop,
        };

        enum
        {
            ID_TESTED_CONTROL_00 = 1007,
        };

        int currentPanelState;
        //Twins::PanelState panelState;
        Ui::Animation animFelix;
        Ui::Animation animLaptop;
        Ui::Animation animRecylcer;
        Ui::Animation animBallApng;
        Ui::Animation animBallSvg;

        Tests(HWND owner, CRect& rc)
            : currentPanelState(0)
            , animFelix(MAKEINTRESOURCE(IDR_ANIM_FELIX), _T("ANIMS"))
            , animLaptop(MAKEINTRESOURCE(IDR_ANIM_LAPTOP), _T("ANIMS"))
            , animRecylcer(MAKEINTRESOURCE(IDR_ANIM_RECYCLER), _T("ANIMS"))
            , animBallApng(MAKEINTRESOURCE(IDR_ANIM_BALL_PNG), _T("ANIMS"))
            , animBallSvg(MAKEINTRESOURCE(IDR_ANIM_BALL_SVG), _T("ANIMS"))
        {
            panelState.Create(owner, rc, NULL, 0, 0, ID_TESTED_CONTROL_00);
            SwitctStateTo(currentPanelState);
        }

        void SwitctStateTo(int newState)
        { 
            panelState.Hide();

            currentPanelState = newState;

            switch (currentPanelState)
            {
            case BallApng:
                panelState.SetAnimation(animBallApng);
                panelState.CaptionText() = L"Animated PNG";
                panelState.ActionText() = L"Переключить на SVG";
                panelState.Action() = boost::bind(&Tests::SwitctStateTo, this, BallSvg);
                break;

            case BallSvg:
                panelState.SetAnimation(animBallSvg);
                panelState.CaptionText() = L"Animated SVG";
                panelState.ActionText() = L"Переключить на WithoutAnimation";
                panelState.Action() = boost::bind(&Tests::SwitctStateTo, this, WithoutAnimation);
                break;

            case WithoutAnimation:
                panelState.RemoveAnimation();
                panelState.CaptionText() = L"Без анимации...";
                panelState.ActionText() = L"Переключить на феликса";
                panelState.Action() = boost::bind(&Tests::SwitctStateTo, this, Felix);
                break;

            case Felix:
                panelState.SetAnimation(animFelix);
                panelState.CaptionText() = L"Прогулка Феликса";
                panelState.ActionText() = L"Пнуть по жопе";
                panelState.Action() = boost::bind(&Tests::SwitctStateTo, this, Laptop);
                break;

            case Laptop:
                panelState.SetAnimation(animLaptop);
                panelState.CaptionText() = L"Поиск хостов в сети";
                panelState.ActionText() = L"Отмена";
                panelState.Action() = boost::bind(&Tests::SwitctStateTo, this, Recycler);
                break;

            case Recycler:
                panelState.SetAnimation(animRecylcer);
                panelState.CaptionText() = L"Загрузка...";
                panelState.ActionText() = L"Отмена";
                panelState.Action() = boost::bind(&Tests::SwitctStateTo, this, WithoutAnimation);
                break;
            }

            panelState.Show();
        }
    };

    void PanelStateTest(CAppModule&, CMessageLoop& loop)
    {
        HLWindow window(loop);
        if (!window.Create())
        {
            DropFailBox(_T("Ошибка при создании")
                , _T("Сбой HLWindow::Create() - %s")
                , Str::ErrorCode<TCHAR>::SystemMessage(::GetLastError()));

            return ;
        }

        HLControl& control = window.GetHlControl();
        CRect rc;
        ::GetClientRect(control.m_hWnd, rc);

        Tests tests(control.m_hWnd, rc);

        window.ShowWindow(SW_SHOW);
        window.UpdateWindow();

        loop.Run();
    }
}
#endif
