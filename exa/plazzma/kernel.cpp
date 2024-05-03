#include "stdafx.h"
#include "kernel.h"
#include "gfx.h"
#include "ereport.h"
#include <stdlib.h>
#include <math.h>

static const double pi = 3.1415926535897932384626433832785;
static Bytes buffer1 = NULL;
static Bytes buffer2 = NULL;
static Bytes blender = NULL;
static int pcx = 0;
static int pcy = 0;

bool PlazzmaInit(int cx, int cy)
{
    PlazzmaDestroy();

    pcx = cx;
    pcy = cy;

    size_t bs = cx * cy;

    buffer1 = (Bytes)::calloc(1, bs);
    elogMessage(buffer1 ? 0 : ERROR_NOT_ENOUGH_MEMORY, "Allocating %d bytes 4 buffer1", bs);

    if (buffer1)
    {
        buffer2 = (Bytes)::calloc(1, bs);
        elogMessage(buffer2 ? 0 : ERROR_NOT_ENOUGH_MEMORY, "Allocating %d bytes 4 buffer2", bs);

        if (buffer2)
        {
            blender = (Bytes)::calloc(1, bs / 4);
            elogMessage(buffer2 ? 0 : ERROR_NOT_ENOUGH_MEMORY, "Allocating %d bytes 4 blender", bs/4);
        }
    }

    return buffer1 && buffer2 && blender;
}

void PlazzmaDestroy()
{
    if (buffer1) 
    {
        elogMessage("deallocate buffer1", 0);
        ::free((void*)buffer1);
        buffer1 = NULL;
    }

    if (buffer2) 
    {
        elogMessage("deallocate buffer2", 0);
        ::free((void*)buffer2);
        buffer2 = NULL;
    }

    if (blender) 
    {
        elogMessage("deallocate blender", 0);
        ::free((void*)blender);
        blender = NULL;
    }
}

static void GenBuffer1()
{
    int midx = pcx / 2;
    int midy = pcy / 2;

    for (int i = 0; i < pcy; i++) 
    {
        for (int j = 0; j < pcx; j++) 
        {
            buffer1[(i * pcx) + j] = (Byte)((sqrt(16.+(midx-i)*(midy-i)+(midx-j)*(midx-j))-4)*5);
        }
    }
}

static void GenBuffer2()
{
    int midx = pcx / 2;
    int midy = pcy / 2;

    for (int i = 0; i < pcy; i++) 
    {
        for (int j = 0; j < pcx; j++) 
        {
            double temp = sqrt(16. + (midy-i) * (midy-i) + (midx-j) * (midx-j)) - 4;
            buffer2[(i * pcx) + j] = (Byte)((sin(temp / 9.5) + 1) * 90.);
        }
    }
}

static double circle1 = 0, circle2 = 0, circle3 = 0, circle4 = 0
            , circle5 = 0, circle6 = 0, circle7 = 0, circle8 = 0;

static int x1, x2, x3, x4, Y1, y2, y3, y4, roll = 0;

static void BlendBuffers()
{
    circle1 += .085 / 6;
    circle2 -= .1   / 6;
    circle3 += .3   / 6;
    circle4 -= .2   / 6;
    circle5 += .4   / 6;
    circle6 -= .15  / 6;
    circle7 += .35  / 6;
    circle8 -= .05  / 6;

    x2 = (int)((pcx/4) + (pcx/4) * sin(circle1));
    y2 = (int)((pcy/4) + (pcy/4) * cos(circle2));
    x1 = (int)((pcx/4) + (pcx/4) * cos(circle3));
    Y1 = (int)((pcy/4) + (pcy/4) * sin(circle4));
    x3 = (int)((pcx/4) + (pcx/4) * cos(circle5));
    y3 = (int)((pcy/4) + (pcy/4) * sin(circle6));
    x4 = (int)((pcx/4) + (pcx/4) * cos(circle7));
    y4 = (int)((pcy/4) + (pcy/4) * sin(circle8));

    int midx = pcx / 2;
    int midy = pcy / 2;

    int k = 0;
    for (int i = 0; i < midy; i++) 
    {
        for (int j = 0; j < midx; j++) 
        {
            blender[k + j] = buffer1[pcx * (i + Y1) + j + x1] + roll +
                             buffer2[pcx * (i + y2) + j + x2] +
                             buffer2[pcx * (i + y3) + j + x3] +
                             buffer2[pcx * (i + y4) + j + x4];

            if (blender[k + j] > 255) 
                blender[k + j] = 255;
        }

        k += midx;
    }
}


void PlazzmaCalc()
{
    GenBuffer1();
    GenBuffer2();

#ifdef _DEBUG
    gfxDrop(buffer1, 0, 0, pcx, pcy);
    gfxDrop(buffer2, pcx + 2, 0, pcx, pcy);
#endif
}

void PlazzmaDefaultPalette(PALETTEENTRY* palette, int count)
{
    int midc = count / 2;
    double step = count / 256.;
    double value = 64;

    for (int i=0; i<count; i++)
    {
        palette[i].peRed = (BYTE)(value / 1.5);
        palette[i].peGreen = (BYTE)(value / 2.5);
        palette[i].peBlue = (BYTE)(value);
        palette[i].peFlags = 0;

        if (i < midc)
            value += step;
        else
            value -= step;
    }
}

void PlazzmaBlender()
{
    BlendBuffers();
    gfxFitDrop(blender, 0, 0, pcx / 2, pcy / 2);
}
