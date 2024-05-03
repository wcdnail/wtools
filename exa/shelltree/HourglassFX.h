#if !defined(AFX_HOURGLASSFX_H__C53DB5BF_841A_44D4_8C41_062A9CFDD449__INCLUDED_)
#define AFX_HOURGLASSFX_H__C53DB5BF_841A_44D4_8C41_062A9CFDD449__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HourglassFX.h : header file
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

/////////////////////////////////////////////////////////////////////////////
// CHourglassFX window

template <class BASE_TYPE>
class CHourglassFX : public BASE_TYPE
{
private:
	enum { ANICURSOR_FRAMES = 10 };

	HBRUSH m_hBrush;
	CRect m_rcIcon;
	int m_iAniStep;
	HCURSOR m_hAniCur;

// Construction-Destruction
public:
	CHourglassFX()
	{
		ShowWaitMessage();
		SetAnimationDelay(125);

		m_hAniCur = AfxGetApp()->LoadCursor(IDR_HOURGLASS);
	};

	virtual ~CHourglassFX()
	{
		DestroyCursor(m_hAniCur);
	};

// Implementation
protected:
	virtual void PreAnimation(HTREEITEM hItemMsg);
	virtual void DoAnimation(BOOL bTimerEvent, int iMaxSteps, int iStep);
	virtual void PostAnimation();
};

/////////////////////////////////////////////////////////////////////////////
// CHourglassFX

template <class BASE_TYPE>
void CHourglassFX<BASE_TYPE>::PreAnimation(HTREEITEM hItemMsg)
{
	BASE_TYPE::PreAnimation(hItemMsg);

	m_iAniStep = 0;

	// get msg image rect
	GetItemImageRect(hItemMsg, &m_rcIcon);

	// create background brush with current bg color (take rgb part only)
	m_hBrush = CreateSolidBrush(GetBkColor() & 0x00FFFFFF);
}

template <class BASE_TYPE>
void CHourglassFX<BASE_TYPE>::PostAnimation()
{
	DeleteObject(m_hBrush);

	BASE_TYPE::PostAnimation();
}

template <class BASE_TYPE>
void CHourglassFX<BASE_TYPE>::DoAnimation(BOOL bTimerEvent, int iMaxSteps, int iStep)
{
	BASE_TYPE::DoAnimation(bTimerEvent, iMaxSteps, iStep);

	if (!bTimerEvent) return;	// process only timer events

	m_iAniStep = (m_iAniStep + 1) % ANICURSOR_FRAMES;

	CClientDC dc(this);

	DrawIconEx(dc.GetSafeHdc(), m_rcIcon.left, m_rcIcon.top, m_hAniCur,
		m_rcIcon.Width(), m_rcIcon.Height(), m_iAniStep, m_hBrush, DI_NORMAL);
}


#endif // !defined(AFX_HOURGLASSFX_H__C53DB5BF_841A_44D4_8C41_062A9CFDD449__INCLUDED_)
