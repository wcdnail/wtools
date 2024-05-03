#include "StdAfx.h"
#include "Application.h"
#include "ErrorReport.h"
#include "UI.h"

namespace UI
{
	//
	//
	// Additional functions
	//
	//

	IDirect3DDevice9 * g_pD3DDev = NULL;

	void GrowRect(RECT& rc, LONG x, LONG y)
	{
		if (x) { rc.left -= x; rc.right  += (x * 2); }
		if (y) { rc.top  -= y; rc.bottom += (y * 2); }
	}

	const RECT& ClientRect()
	{
		static RECT rcClient;
		::GetClientRect(GetApp().WindowHandle(), &rcClient);
		return rcClient;
	}

	const RECT& CenteredRect(float fXPercent, float fYPercent)
	{
		static RECT rc;
		const RECT& rcClient = ClientRect();

		rc.right = (LONG)( (float)rcClient.right * (fXPercent/100.0f) );
		rc.bottom = (LONG)( (float)rcClient.bottom * (fYPercent/100.0f) );
		rc.left = (rcClient.right - rc.right)/2;
		rc.top = (rcClient.bottom - rc.bottom)/2;

		return rc;
	}

	bool InitControls()
	{
		EnterFunc();

		g_pD3DDev = GetApp().GetD3DDevice();
		if ( !g_pD3DDev )
		{
			AddErrorMessage( L"GetApp().GetD3DDevice() is NULL" );
			return false;
		}

		return true;
	}

	//
	//
	// Control base class
	//
	//

	inline void Control::Vertex::Set(float a, float b)
	{
		x = a;
		y = b;
	}

	inline void Control::Vertex::SetUV(float a, float b)
	{
        u = a;
		v = b;
	}

	Control::Control()
		: m_pVB			( NULL )
		, m_pTex		( NULL )
		, m_nVertCount	( 4 )
	{
		Defaults();
	}

	Control::Control(int nVertCount)
		: m_pVB			( NULL )
		, m_pTex		( NULL )
		, m_nVertCount	( nVertCount )
	{
		Defaults();
	}

	Control::~Control()
	{
		SAFE_RELEASE( m_pVB );
	}

	void Control::Defaults()
	{
		HRESULT hr = g_pD3DDev->CreateVertexBuffer( SizeOfVB(), 0, FVF, D3DPOOL_MANAGED, &m_pVB, NULL);

		if (FAILED(hr))
		{
			AddWarningMessage( L"CreateVertexBuffer failed" );
		}

		ZeroRect();
	}

	void Control::ZeroRect()
	{
		Vertex * pVert;
		if( SUCCEEDED( m_pVB->Lock( 0, SizeOfVB(), (void**)&pVert, 0 ) ) )
		{
			memset( pVert, 0, SizeOfVB() );

			pVert[0].SetUV(0, 0); pVert[0].color = 0xffffffff;
			pVert[1].SetUV(1, 0); pVert[1].color = 0xffffffff;
			pVert[2].SetUV(1, 1); pVert[2].color = 0xffffffff;
			pVert[3].SetUV(0, 1); pVert[3].color = 0xffffffff;

			m_pVB->Unlock();
		}
	}

	#pragma warning (push)
	#pragma warning (disable : 4244)

	void Control::SetRect(const RECT& rcSource)
	{
		Vertex *pVert;
		if( SUCCEEDED( m_pVB->Lock( 0, SizeOfVB(), (void**)&pVert, 0 ) ) )
		{
			RECT rc = rcSource;
			rc.right += rc.left;
			rc.bottom += rc.top;

			pVert[0].Set(rc.left, rc.top);
			pVert[1].Set(rc.right, rc.top);
			pVert[2].Set(rc.right, rc.bottom);
			pVert[3].Set(rc.left, rc.bottom);

			m_pVB->Unlock();
		}
	}

	void Control::SetRect(float x, float y, float w, float h)
	{
		RECT rc = { x, y, w, h };
		SetRect( rc );
	}

	RECT Control::GetRect() const
	{
		RECT rc = { 0, 0, 0, 0 };

		Vertex *pVert;
		if( SUCCEEDED( m_pVB->Lock( 0, SizeOfVB(), (void**)&pVert, 0 ) ) )
		{
			rc.left = pVert[0].x;
			rc.top = pVert[0].y;
			rc.right = pVert[2].x - rc.left;
			rc.bottom = pVert[2].y - rc.top;

			m_pVB->Unlock();
		}

		return rc;
	}

	void Control::SetRect(const Control& CtlRef)
	{
		RECT rc = CtlRef.GetRect();
		SetRect( rc );
	}

	void Control::SetColor(DWORD dwColor)
	{
		Vertex *pVert;
		if( SUCCEEDED( m_pVB->Lock( 0, SizeOfVB(), (void**)&pVert, 0 ) ) )
		{
			pVert[0].color = dwColor;
			pVert[1].color = dwColor;
			pVert[2].color = dwColor;
			pVert[3].color = dwColor;
			m_pVB->Unlock();
		}
	}

	void Control::SetColor(DWORD dwColor1, DWORD dwColor2, bool bVertical)
	{
		Vertex *pVert;
		if( SUCCEEDED( m_pVB->Lock( 0, SizeOfVB(), (void**)&pVert, 0 ) ) )
		{
			if (bVertical)
			{
				pVert[0].color = dwColor1;
				pVert[1].color = dwColor1;
				pVert[2].color = dwColor2;
				pVert[3].color = dwColor2;
			}
			else
			{
				pVert[0].color = dwColor1;
				pVert[1].color = dwColor2;
				pVert[2].color = dwColor2;
				pVert[3].color = dwColor1;
			}

			m_pVB->Unlock();
		}
	}

	#pragma warning (pop)

	void Control::Display()
	{
		if (NULL != m_pTex)
			g_pD3DDev->SetTexture(0, m_pTex->GetObject());

		g_pD3DDev->SetStreamSource( 0, m_pVB, 0, sizeof(Vertex) );
		g_pD3DDev->SetFVF( FVF );
		g_pD3DDev->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
	}

	//
	//
	// Rectangle
	//
	//

	void Rectangle::SetRect(const RECT& rcSource)
	{
		Control::SetRect( rcSource );

		Vertex *pVert;
		if( SUCCEEDED( m_pVB->Lock( 0, SizeOfVB(), (void**)&pVert, 0 ) ) )
		{
			pVert[4] = pVert[0];
			m_pVB->Unlock();
		}
	}

	void Rectangle::Display()
	{
		g_pD3DDev->SetStreamSource( 0, m_pVB, 0, sizeof(Vertex) );
		g_pD3DDev->SetFVF( FVF );
		g_pD3DDev->DrawPrimitive( D3DPT_LINESTRIP, 0, m_nVertCount - 1 );
	}
}
