#pragma once

#include "TextureGenerator.h"

namespace TexGen
{
	class ConfigDialog
	{
	public:
		ConfigDialog( UINT );
		~ConfigDialog();
		void Show();
		void Hide();
		void Move( const RECT&, BOOL = TRUE );
		void SetObjects( PlasmaCircles &, PlasmaCircles &, PlasmaBlender & );

	private:
		void Create(int);
		static LRESULT CALLBACK WinProc( HWND, UINT, WPARAM, LPARAM );
		static void ChangePlasmaAlpha( double );

		HWND m_hWnd;
		UINT m_nID;

		PlasmaCircles			* m_pA;
		PlasmaCircles			* m_pB;
		PlasmaBlender	* m_pC;
	};

	inline void ConfigDialog::SetObjects( PlasmaCircles & A, PlasmaCircles & B, PlasmaBlender & C )
	{
		m_pA = &A;
		m_pB = &B;
		m_pC = &C;
	}
}
