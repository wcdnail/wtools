#pragma once

#include <wcdafx.api.h>
#include <winuser.h>

enum ENCM_Index : int
{
    /*                    */ NCM_Invalid = -1,
    /* NONCLIENTMETRICS   */
    /* iBorderWidth       */ NCM_Border = 0,
    /* iScrollWidth       */ NCM_ScrollWidth,
    /* iScrollHeight      */ NCM_ScrollHeight,
    /* iCaptionWidth      */ NCM_CaptionWidth,
    /* iCaptionHeight     */ NCM_CaptionHeight,
    /* iSmCaptionWidth    */ NCM_SMCaptionWidth,
    /* iSmCaptionHeight   */ NCM_SMCaptionHeight,
    /* iMenuWidth         */ NCM_MenuWidth,
    /* iMenuHeight        */ NCM_MenuHeight,
    /* iPaddedBorderWidth */ NCM_PaddedBorder,
    /*                    */ NCM_Count
};

struct CNCMetrics: NONCLIENTMETRICS
{
    DELETE_COPY_MOVE_OF(CNCMetrics);

    struct Range
    {
        int     min;
        int     max;
        int current;
    };

    ~CNCMetrics();
    CNCMetrics();

    static Range const& DefaultRange(int index);
    static PCWSTR Title(int index);

    bool LoadDefaults();
    void CopyTo(CNCMetrics& target) const noexcept;

    int& operator[](int index);
    int operator[](int index) const;

private:
    template <typename ReturnType, typename SelfRef>
    static ReturnType& getRefByIndex(SelfRef& thiz, int index);
};
