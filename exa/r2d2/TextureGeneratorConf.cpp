#include "StdAfx.h"
#include "Application.h"
#include "Texture.h"
#include "TextureGeneratorConf.h"
#include "ErrorReport.h"
#include "DirectExtra.h"

namespace TexGen
{
	ConfigDialog * g_pPlasmaConf;

	ConfigDialog::ConfigDialog( UINT nID )
		: m_hWnd	( NULL )
		, m_nID		( nID )
		, m_pA		( NULL )
		, m_pB		( NULL )
		, m_pC		( NULL )
	{
	}

	ConfigDialog::~ConfigDialog()
	{
	}

	const float FixedFloatCoef = 100.0f;

	struct PlasmaSlider
	{
		HWND hCaption;
		HWND hSlider;
		HWND hEdit;
		double *pdVal;
		LONG nPos;
		PlasmaCircles *	pGen;
		void (*pfnOnChange)(double);

		PlasmaSlider()
		    : hCaption(NULL)
		    , hSlider(NULL)
		    , hEdit(NULL)
		    , pdVal(NULL)
		    , nPos(0)
		    , pGen(NULL)
		    , pfnOnChange(NULL)
        {}

		void Init( PlasmaCircles *, double *, double = -100, double = 100 );
		void SetText();
		void GetFromControl();
		void GetFromObject();
	};


	void PlasmaSlider::SetText()
	{
		if (NULL != hEdit)
		{
			wchar_t szVal[64];
			_snwprintf( szVal, 64, L"%0.2f", *pdVal );
			::SetWindowTextW( hEdit, szVal );
		}
	}

	void PlasmaSlider::GetFromControl()
	{
		if (NULL != hSlider)
		{
			UINT nLast = nPos;
			nPos = ::SendMessage( hSlider, TBM_GETPOS, 0, 0 );
			if ( nLast != nPos )
			{
				*pdVal = nPos / FixedFloatCoef;
				SetText();
				if ( pGen ) pGen->Generate();
				if ( pfnOnChange ) pfnOnChange( *pdVal );
			}
		}
	}

	void PlasmaSlider::Init(PlasmaCircles * pGenerator, double * pdVallue, double nMin, double nMax)
	{
		pGen = pGenerator;
		pdVal = pdVallue;
		if (NULL != hSlider)
		{
			::SendMessage( hSlider, TBM_SETRANGEMIN, FALSE, nMin * FixedFloatCoef );
			::SendMessage( hSlider, TBM_SETRANGEMAX, TRUE, nMax * FixedFloatCoef );
		}
		GetFromObject();
	}

	void PlasmaSlider::GetFromObject()
	{
		if ( pdVal && hSlider)
		{
			nPos = *pdVal;
			SetText();
			::SendMessage( hSlider, TBM_SETPOS, TRUE, nPos * FixedFloatCoef );
		}
	}

	#define SetupCParameter(nID, V) \
		V.hCaption = GetDlgItem( hWnd, IDC_##nID##_CP ); \
		V.hSlider  = GetDlgItem( hWnd, IDC_##nID##_SL ); \
		V.hEdit	   = GetDlgItem( hWnd, IDC_##nID##_ED ); 

	#define AddValue2Combo( X ) \
		i =	::SendMessage( hCombo, CB_ADDSTRING, 0, (LPARAM)IdentToString(X) ); \
			::SendMessage( hCombo, CB_SETITEMDATA, i, (LPARAM)X );


	void InitAlgorithmCombo( HWND hCombo )
	{
		::SendMessage( hCombo, CB_RESETCONTENT, 0, 0 ); 
		int i = 0;
		AddValue2Combo( CONCENTRIC_SMOOTH_CIRCLES );
		AddValue2Combo( CONCENTRIC_SHARPEDGE_CIRCLES );
		AddValue2Combo( HORIZONTAL_GRADIENT );
		AddValue2Combo( VERTICAL_GRADIENT );
		AddValue2Combo( DIAGONAL_GRADIENT );
	}

	void ConfigDialog::ChangePlasmaAlpha(double dNewValue)
	{
		if (g_pPlasmaConf) g_pPlasmaConf->m_pC->SetAlpha(dNewValue);
	}

	LRESULT CALLBACK ConfigDialog::WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		enum { MAX_SLIDERS = 10 };
		static PlasmaSlider slider[MAX_SLIDERS];
		static HWND algoCombo[2] = { NULL, NULL };
		static HWND paletteCheck[4] = { NULL, NULL, NULL, NULL };
		static double plasmaAlpha = 255.0f;
		static bool useRed = true, useGreen = false, useBlue = false;

		for ( int i=0; i<MAX_SLIDERS; i++ )
		{
			if (lParam == (LPARAM)slider[i].hSlider)
			{
				slider[i].GetFromControl();
				break;
			}
		}

		switch ( uMsg )
		{
		case WM_INITDIALOG:
		{
			if ( !g_pPlasmaConf ) return FALSE;

			SetupCParameter(M_DSTART1,      slider[0]);
			SetupCParameter(M_DRADIUS1,     slider[1]);
			SetupCParameter(M_CENTERX1,     slider[2]);
			SetupCParameter(M_CENTERY1,     slider[3]);
			SetupCParameter(M_DSTART2,      slider[4]);
			SetupCParameter(M_DDIVISOR,     slider[5]);
			SetupCParameter(M_DCONSISTENCE, slider[6]);
			SetupCParameter(M_CENTERX2,     slider[7]);
			SetupCParameter(M_CENTERY2,     slider[8]);

			slider[9].hSlider = ::GetDlgItem( hWnd, IDC_PLASMAALPHA );
			slider[9].Init( NULL, &plasmaAlpha, 0, 255 );
			slider[9].pfnOnChange = &ChangePlasmaAlpha;

			PlasmaCircles *	pGen1 = g_pPlasmaConf->m_pA;
			PlasmaCircles *	pGen2 = g_pPlasmaConf->m_pB;
			UINT		nWidth1 = (UINT)(UINT)pGen1->GetParams().centerx * 2;
			UINT		nWidth2 = (UINT)(UINT)pGen2->GetParams().centerx * 2;
			UINT		nHeight1 = (UINT)(UINT)pGen1->GetParams().centery * 2;
			UINT		nHeight2 = (UINT)(UINT)pGen2->GetParams().centery * 2;

			slider[0].Init(pGen1, &pGen1->GetParams().start,         -1000, 1000);
			slider[1].Init(pGen1, &pGen1->GetParams().radius,        -10, 10);
			slider[2].Init(pGen1, &pGen1->GetParams().centerx,       0, nWidth1);
			slider[3].Init(pGen1, &pGen1->GetParams().centery,       0, nHeight1);
			slider[4].Init(pGen2, &pGen2->GetParams().start,         -1000, 1000);
			slider[5].Init(pGen2, &pGen2->GetParams().divisor,       0.01, nWidth2/2);
			slider[6].Init(pGen2, &pGen2->GetParams().consistence,   0, 127);
			slider[7].Init(pGen2, &pGen2->GetParams().centerx,       0, nWidth2);
			slider[8].Init(pGen2, &pGen2->GetParams().centery,       0, nHeight2);

			algoCombo[0] = ::GetDlgItem( hWnd, IDC_ALGO1_COMBO );
			InitAlgorithmCombo( algoCombo[0] );
			::SendMessage( algoCombo[0], CB_SETCURSEL, pGen1->GetAlgorithm(), 0 );

			algoCombo[1] = ::GetDlgItem( hWnd, IDC_ALGO2_COMBO );
			InitAlgorithmCombo( algoCombo[1] );
			::SendMessage( algoCombo[1], CB_SETCURSEL, pGen2->GetAlgorithm(), 0 );

			PlasmaBlender * pBlender = g_pPlasmaConf->m_pC;

			paletteCheck[0] = ::GetDlgItem( hWnd, IDC_SPILLOVER );
			paletteCheck[1] = ::GetDlgItem( hWnd, IDC_PLASMARED );
			paletteCheck[2] = ::GetDlgItem( hWnd, IDC_PLASMAGREEN );
			paletteCheck[3] = ::GetDlgItem( hWnd, IDC_PLASMABLUE );

			::SendMessage( paletteCheck[0], BM_SETCHECK, ( pBlender->Spillover() ? BST_CHECKED : BST_UNCHECKED ), 0 );
			::SendMessage( paletteCheck[1], BM_SETCHECK, ( useRed ? BST_CHECKED : BST_UNCHECKED ), 0 );
			::SendMessage( paletteCheck[2], BM_SETCHECK, ( useGreen ? BST_CHECKED : BST_UNCHECKED ), 0 );
			::SendMessage( paletteCheck[3], BM_SETCHECK, ( useBlue ? BST_CHECKED : BST_UNCHECKED ), 0 );

			return TRUE;
		}

		case WM_COMMAND:

			switch ( HIWORD(wParam) )
			{
			case CBN_SELCHANGE:
			{
				PlasmaCircles *pSimple = g_pPlasmaConf->m_pA;
				HWND hCombo = (HWND)lParam;
				if (lParam == (LPARAM)algoCombo[1]) pSimple = g_pPlasmaConf->m_pB;
				
				int nIndex = ::SendMessage(hCombo, CB_GETCURSEL, 0, 0);
				int Algo = ::SendMessage(hCombo, CB_GETITEMDATA, nIndex, 0);

				pSimple->SelectAlgorithm( Algo );
				pSimple->Generate();
				
				break;
			}
			}

			switch ( LOWORD(wParam) )
			{
			case IDC_SPILLOVER: 
			{
				int nCheck = ::SendMessage( paletteCheck[0], BM_GETCHECK, 0, 0 );
				if		( BST_CHECKED == nCheck ) g_pPlasmaConf->m_pC->SpilloverOn();
				else if ( BST_UNCHECKED == nCheck ) 
				{
					g_pPlasmaConf->m_pC->SpilloverOff();
					goto SetPalette;
				}
				break;
			}

			case IDC_PLASMARED:		
				useRed = ::SendMessage( paletteCheck[1], BM_GETCHECK, 0, 0 ) == BST_CHECKED;
				goto SetPalette;

			case IDC_PLASMAGREEN:	
				useGreen = ::SendMessage( paletteCheck[2], BM_GETCHECK, 0, 0 ) == BST_CHECKED;
				goto SetPalette;

			case IDC_PLASMABLUE:	
				useBlue = ::SendMessage( paletteCheck[3], BM_GETCHECK, 0, 0 ) == BST_CHECKED;
SetPalette:
				g_pPlasmaConf->m_pC->DefaultPalette( useRed, useGreen, useBlue, (UINT)plasmaAlpha );
				break;

			case IDCANCEL: break;
			case IDOK: break;

			case IDC_RESETBTN:
				if ( g_pPlasmaConf )
				{
					g_pPlasmaConf->m_pA->Defaults();
					g_pPlasmaConf->m_pA->Generate();
					g_pPlasmaConf->m_pB->Defaults();
					g_pPlasmaConf->m_pB->Generate();

					for ( int i=0; i<MAX_SLIDERS; i++ )
					{
						slider[i].GetFromObject();
					}
				}
			}

			break;

			//::SendMessage( GetApp().WindowHandle(), uMsg, wParam, lParam );

		}

		return FALSE;
	}

	void ConfigDialog::Create(int nID)
	{
		m_hWnd = ::CreateDialog(  GetApp().ModuleHandle()
								, MAKEINTRESOURCE(nID)
								, GetApp().WindowHandle()
								, (DLGPROC)WinProc
								);
	}

	void ConfigDialog::Show()
	{
		if ( !m_pA || !m_pB || !m_pC )
		{
			AddErrorMessage ( L"<%S>", __FUNCTION__ );
			AddNoticeMessage( L"не инициированны ссылки на объекты");
			AddNoticeMessage( L"или объекты не созданы" );
			return ;
		}

		g_pPlasmaConf = this;
        if ( NULL == m_hWnd ) Create( m_nID );
		::ShowWindow( m_hWnd, SW_SHOWNORMAL );
	}

	void ConfigDialog::Hide()
	{
		if ( NULL != m_hWnd ) 
			::ShowWindow( m_hWnd, SW_HIDE );
	}

	void ConfigDialog::Move( const RECT& rc, BOOL bRepaint )
	{
		if ( NULL != m_hWnd ) 
			::MoveWindow( m_hWnd, rc.left, rc.top, rc.right, rc.bottom, bRepaint );
	}
}
