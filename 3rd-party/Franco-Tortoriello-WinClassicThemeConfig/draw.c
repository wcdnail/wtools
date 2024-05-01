/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Provides general utility drawing functions.
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "draw.h"

_Success_(return)
BOOL DrawBorder(HDC hdc, _In_ const RECT *pRect, int borderWidth, HBRUSH hBrush)
{
    BOOL ret = TRUE;
    RECT rc = *pRect;
    int length;

    HBRUSH hPreviousBrush = SelectObject(hdc, hBrush);
    if (!hPreviousBrush)
        return FALSE;

    /* Left */
    length = rc.bottom - rc.top;
    ret &= PatBlt(hdc, rc.left, rc.top, borderWidth, length, PATCOPY);
    rc.left += borderWidth;

    /* Right */
    rc.right -= borderWidth;
    ret &= PatBlt(hdc, rc.right, rc.top, borderWidth, length, PATCOPY);

    /* Top */
    length = rc.right - rc.left;
    ret &= PatBlt(hdc, rc.left, rc.top, length, borderWidth, PATCOPY);
    rc.top += borderWidth;

    /* Bottom */
    rc.bottom -= borderWidth;
    ret &= PatBlt(hdc, rc.left, rc.bottom, length, borderWidth, PATCOPY);

    SelectObject(hdc, hPreviousBrush);
    return ret;
}
