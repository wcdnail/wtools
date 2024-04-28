#pragma once

#include "generic.div.h"
#include <atlctrls.h>

namespace CF
{
    template <class T>
    class LargeProgress
    {
    public:
        ~LargeProgress();
        LargeProgress(CProgressBarCtrl& ui);

        void Range(T mn, T mx);
        void UpdateControl() const;
        LargeProgress& operator = (T pos);
        T Get() const;

    private:
        T Pos;
        T Min;
        T Max;
        CProgressBarCtrl& Ui;
    };

    template <class T>
    inline LargeProgress<T>::~LargeProgress() 
    {}

    template <class T>
    inline LargeProgress<T>::LargeProgress(CProgressBarCtrl& ui) 
        : Pos(0)
        , Min(0)
        , Max(0)
        , Ui(ui)
    {}

    template <class T>
    inline void LargeProgress<T>::Range(T mn, T mx)
    {
        Min = mn, Max = mx;
    }

    template <class T>
    inline void LargeProgress<T>::UpdateControl() const
    {
        int mn = 0, mx = 0;
        Ui.GetRange(mn, mx);

        double c = SafeDiv((double)Pos, (double)(Max - Min));
        double p = c * (double)(mx - mn);
        
        Ui.SetPos((int)p);
    }

    template <class T>
    LargeProgress<T>& LargeProgress<T>::operator = (T pos)
    {
        Pos = pos;
        UpdateControl();
        return *this;
    }

    template <class T>
    T LargeProgress<T>::Get() const
    {
        return Pos;
    }
}
