#pragma once

#include "Base.h"
#include "Texture.h"

namespace UI
{
	//
	//
	// Additional functions
	//
	//

	extern bool			InitControls();
	extern const RECT&	ClientRect	();
	extern const RECT&	CenteredRect( float, float );
	inline const RECT&	CenteredRect( float fPercent ) { return CenteredRect(fPercent, fPercent); }
	extern void			GrowRect	( RECT&, LONG, LONG );

	//
	//
	// Control base class
	//
	//

	class Control : public IDrawable, public IListElement<Control>
	{
	public:
		struct Vertex
		{
			float x, y, z, rhw;
            DWORD color;
			float u, v;
			void Set(float, float);
			void SetUV(float, float);
		};

		enum { FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 };

		Control();
		Control(int);
		~Control();

		void			ZeroRect	();
		virtual void	SetRect		(const RECT&);
		void			SetRect		(float, float, float, float);
		void			SetRect		(const Control&);
		virtual RECT	GetRect		() const;
		void			SetColor	(DWORD);
		void			SetColor	(DWORD, DWORD, bool);
		virtual void	Display		();
		void			SetTexture	(DXTexture * pTex) { m_pTex = pTex; }

	protected:
		void			Defaults	();
		UINT			SizeOfVB	() const { return sizeof(Vertex) * m_nVertCount; }

		IDirect3DVertexBuffer9 *	m_pVB;
		DXTexture *					m_pTex;
		UINT						m_nVertCount;
	};

	//
	//
	// Rectangle
	//
	//

	class Rectangle: public Control
	{
	public:
		Rectangle(): Control( 5 ) {}
		~Rectangle() {}
		void SetRect(const RECT&);
        void Display();
	};

	//
	//
	// Text
	//
	//

	class Text: public IDrawable
	{
	public:
		Text() {}
		~Text() {}
	private:
	};
}
