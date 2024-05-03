#if !defined(AFX_PROGRESSFX_H__3A32359E_4128_426B_AB73_1D74DA101DAA__INCLUDED_)
#define AFX_PROGRESSFX_H__3A32359E_4128_426B_AB73_1D74DA101DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressFX.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000-2001 by Paolo Messina
// (http://www.geocities.com/ppescher - ppescher@yahoo.com)
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

#include <math.h>

/////////////////////////////////////////////////////////////////////////////
// CProgressFX window

template <class BASE_TYPE>
class CProgressFX : public BASE_TYPE
{
private:
	enum { BORDER_SIZE = 3, PROGRESS_HEIGHT = 16,
			PROGRESS_SIZE = 50, STEP_SIZE = 10 };

	CRect m_rcBorder;
	CRect m_rcProg;
	HBRUSH m_hBorderBrush;
	HBRUSH m_hFillBrush;

// Construction
public:
	CProgressFX()
	{
		ShowWaitMessage();
	};

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CProgressFX() {};

protected:
	virtual void PreAnimation(HTREEITEM hItemMsg);
	virtual void DoAnimation(BOOL bTimerEvent, int iMaxSteps, int iStep);
	virtual void PostAnimation();
};

/////////////////////////////////////////////////////////////////////////////
// CProgressFX

template <class BASE_TYPE>
void CProgressFX<BASE_TYPE>::PreAnimation(HTREEITEM hItemMsg)
{
	BASE_TYPE::PreAnimation(hItemMsg);

	// get msg item rect
	GetItemRect(hItemMsg, &m_rcBorder, TRUE);
	int margin = (m_rcBorder.Height() - PROGRESS_HEIGHT) / 2;
	margin = (margin < 0) ? 0 : margin;
	// set height
	m_rcBorder.top += margin;
	m_rcBorder.bottom -= margin;
	// calc progress rect
	m_rcBorder.OffsetRect(m_rcBorder.Width()+BORDER_SIZE, 0);
	m_rcProg = m_rcBorder;
	m_rcProg.DeflateRect(BORDER_SIZE, BORDER_SIZE);
	// set width
	m_rcProg.right = m_rcProg.left;
	m_rcBorder.right = m_rcBorder.left + BORDER_SIZE*2 + PROGRESS_SIZE;

	// create brushes
	m_hBorderBrush = CreateSolidBrush(RGB(0,0,0));
	m_hFillBrush = CreateSolidBrush(RGB(255,0,0));
}

template <class BASE_TYPE>
void CProgressFX<BASE_TYPE>::PostAnimation()
{
	DeleteObject(m_hBorderBrush);
	DeleteObject(m_hFillBrush);

	BASE_TYPE::PostAnimation();
}

template <class BASE_TYPE>
void CProgressFX<BASE_TYPE>::DoAnimation(BOOL bTimerEvent, int iMaxSteps, int iStep)
{
	BASE_TYPE::DoAnimation(bTimerEvent, iMaxSteps, iStep);

	if (bTimerEvent) return;	// process only redraw events

	CClientDC dc(this);
	FrameRect(dc.GetSafeHdc(), &m_rcBorder, m_hBorderBrush);

	if (iMaxSteps > 0)	// linear progress
		m_rcProg.right = m_rcProg.left + iStep*PROGRESS_SIZE/iMaxSteps;
	else	// endless progress
		m_rcProg.right = m_rcProg.left + (LONG)(PROGRESS_SIZE*(1 + 
			STEP_SIZE*(double)iStep/PROGRESS_SIZE - 
			log(exp(2) + exp(2*STEP_SIZE*(double)iStep/PROGRESS_SIZE) - 1)/2));

	FillRect(dc.GetSafeHdc(), &m_rcProg, m_hFillBrush);
}


#endif // !defined(AFX_PROGRESSFX_H__3A32359E_4128_426B_AB73_1D74DA101DAA__INCLUDED_)
