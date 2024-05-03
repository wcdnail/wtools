#pragma once

enum PlasmaAlgorithms
{
    CONCENTRIC_SMOOTH_CIRCLES = 0
,   CONCENTRIC_SHARPEDGE_CIRCLES
,   HORIZONTAL_GRADIENT
,   VERTICAL_GRADIENT
,   DIAGONAL_GRADIENT
};


struct PlasmaParams
{
    int		algo;
    double	radius;
    double	divisor;
    double	consistence;
    double	start;
    double	width;
    double	height;
    double	centerx;
    double	centery;

    PlasmaParams() 
        : algo(CONCENTRIC_SMOOTH_CIRCLES)
        , radius(0)
        , divisor(0)
        , consistence(0)
        , start(0)
        , width(0)
        , height(0)
        , centerx(0)
        , centery(0)
    {}
};
