#pragma once

#if 0
#include "dialogz.common.h"
#include <atlddx.h>
#include <boost/function.hpp>
#include <boost/thread.hpp>

namespace Twins
{
    class ProgressDialog: CommonDialog
    {
    private:
        typedef CommonDialog Super;
        typedef boost::function<void(void)> ThreadFn;

    public:
        ProgressDialog(PCWSTR caption);
        ~ProgressDialog();

        using Super::DoModal;

        void Close(int result);
        void SetErrorCode(HRESULT hr);
        HRESULT GetErrorCode() const;
        void SetHints(CString const& source, CString const& dest);

        void SetThreadFn(ThreadFn const& function);
        boost::thread& GetThread();

        template <class T>
        void SetCurrentRange(T tmin, T tmax);

        template <class T>
        void SetCurrent(T pos); 

        template <class T>
        void SetTotalRange(T tmin, T tmax);

        template <class T>
        void SetTotal(T pos);

    private:
        CStatic source_;
        CStatic dest_;
        CProgressBarCtrl current_;
        CProgressBarCtrl total_;
        boost::thread worker_;
        ThreadFn function_;
        HRESULT error_;
        double cpd_;
        double tpd_;

        BOOL OnInitDialog(HWND, LPARAM param);

        BEGIN_MSG_MAP_EX(ProgressDialog)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
        END_MSG_MAP()

        template <class T>
        static void GetProgressDivisor(double& divisor, T tmin, T tmax);

        template <class T>
        static int ProgressValue(T x, double divisor);

        static void SetRangeFor(CProgressBarCtrl& ctl, int imin, int imax);
        static void SetPosFor(CProgressBarCtrl& ctl, int pos);

        static const int RangeMax = 1000;
    };

    template <class T>
    inline void ProgressDialog::GetProgressDivisor(double& divisor, T tmin, T tmax)
    {
        if (tmax > RangeMax)
            divisor = (double)(tmax - tmin) / (double)RangeMax;
        else
            divisor = 1.;
    }

    template <class T>
    inline int ProgressDialog::ProgressValue(T x, double divisor)
    {
        return (int)((double)x / divisor);
    }

    template <class T>
    inline void ProgressDialog::SetCurrentRange(T tmin, T tmax)
    {
        GetProgressDivisor<T>(cpd_, tmin, tmax);
        SetRangeFor(current_, ProgressValue<T>(tmin, cpd_), ProgressValue<T>(tmax, cpd_));
    }

    template <class T>
    inline void ProgressDialog::SetCurrent(T pos)
    {
        SetPosFor(current_, ProgressValue<T>(pos, cpd_));
    }

    template <class T>
    inline void ProgressDialog::SetTotalRange(T tmin, T tmax)
    {
        GetProgressDivisor(tpd_, tmin, tmax);
        SetRangeFor(total_, ProgressValue<T>(tmin, tpd_), ProgressValue<T>(tmax, tpd_));
    }

    template <class T>
    inline void ProgressDialog::SetTotal(T pos)
    {
        SetPosFor(total_, ProgressValue<T>(pos, tpd_));
    }
}
#endif // 0
