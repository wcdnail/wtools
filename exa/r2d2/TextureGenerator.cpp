#include "stdafx.h"
#include "TextureGenerator.h"
#include "TextureGeneratorConf.h"
#include "ErrorReport.h"
#include "DirectExtra.h"
#include <math.h>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

namespace TexGen
{
    bool PixelGenerator::OnBegin()	{ return true; }
    void PixelGenerator::OnEnd()		{}
    void PixelGenerator::Write8		(BYTE * pBits, UINT x, UINT y, UINT nLine, UINT nIndex) {}
    void PixelGenerator::Write565		(WORD * pBits, UINT x, UINT y, UINT nLine, UINT nIndex) {}
    void PixelGenerator::Write888		(UINT * pBits, UINT x, UINT y, UINT nLine, UINT nIndex) {}
    void PixelGenerator::Write8888	(UINT * pBits, UINT x, UINT y, UINT nLine, UINT nIndex) {}

    void PixelGenerator::wrap8(void * pBits, UINT x, UINT y, UINT l, UINT i) { Write8((BYTE*)pBits, x, y, l, i); }
    void PixelGenerator::wrap565(void * pBits, UINT x, UINT y, UINT l, UINT i) { Write565((WORD*)pBits, x, y, l, i); }
    void PixelGenerator::wrap888(void * pBits, UINT x, UINT y, UINT l, UINT i) { Write888((UINT*)pBits, x, y, l, i); }
    void PixelGenerator::wrap8888(void * pBits, UINT x, UINT y, UINT l, UINT i) { Write8888((UINT*)pBits, x, y, l, i); }

    void PixelGenerator::SelectPixelFormat(D3DFORMAT d3dFormat)
    {
        switch ( d3dFormat )
        {
        case D3DFMT_R3G3B2:
        case D3DFMT_A8:         writer_ = &PixelGenerator::wrap8; bytesPerPixel_ = 1; break;
        case D3DFMT_X4R4G4B4:
        case D3DFMT_A8R3G3B2:
        case D3DFMT_R5G6B5:
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4:   writer_ = &PixelGenerator::wrap565; bytesPerPixel_ = 2; break;
        case D3DFMT_R8G8B8:     writer_ = &PixelGenerator::wrap888; bytesPerPixel_ = 3; break;
        case D3DFMT_X8R8G8B8:
        case D3DFMT_A8R8G8B8:	writer_ = &PixelGenerator::wrap8888; bytesPerPixel_ = 4; break;
        default:                bytesPerPixel_ = 0;
        }
    }

	void SoftwareGenerator::SelectTexture(DXTexture const& Tex, UINT nLevel)
	{
		dxTex_ = Tex.GetObject();
		texLevel_ = nLevel;
		if (dxTex_) 
		{
			dxTex_->GetLevelDesc(texLevel_, &texDesc_);
			SelectPixelFormat(texDesc_.Format);
		}
	}

	void SoftwareGenerator::Generate()
	{
		if (!dxTex_)
		{
			AddWarningMessage( L"SoftTextureGenerator : целевая текстура = NULL" );
			return;
		}

		if (!bytesPerPixel_ || !writer_)
		{
			AddWarningMessage( L"SoftTextureGenerator : неверный формат текстуры <%s>"
				, D3DFormatToString(texDesc_.Format) );
			return;
		}

		RECT rcLock = { 0, 0, texDesc_.Width, texDesc_.Height };

		if (SUCCEEDED(dxTex_->LockRect(texLevel_, &rcLock_, &rcLock, 0)))
		{
			UINT nWidht		= texDesc_.Width;
			UINT nHeight	= texDesc_.Height;
			BYTE *pBits		= (BYTE*) rcLock_.pBits;
			UINT  nPitch	= rcLock_.Pitch;
			UINT y			= 0;
			UINT nLine		= 0;

			if ( OnBegin() )
			{
				while ( y < nHeight )
				{
					UINT x = 0;
					nLine = y * nWidht;
					while ( x < nWidht )
					{
						((*this).*(writer_))
							( pBits, x, y, nLine, nLine + x );
						++x;
					}
					++y;
				}
			}

			OnEnd();

			dxTex_->UnlockRect( texLevel_ );
		}
	}

	PlasmaCircles::PlasmaCircles()
	{
		generator_ = (PFN_PIXELGEN)&PlasmaCircles::ConcentricSmooth;
		Defaults();
	}

	PlasmaCircles::PlasmaCircles( DXTexture & Tex, UINT nLevel )
		: SoftwareGenerator(Tex, nLevel)
	{
		generator_ = (PFN_PIXELGEN)&PlasmaCircles::ConcentricSmooth;
		Defaults();
	}


	void PlasmaCircles::SelectAlgorithm(int algorithm)
	{
		params_.algo = algorithm;

		switch (algorithm)
		{
		case CONCENTRIC_SMOOTH_CIRCLES:     generator_ = (PFN_PIXELGEN)&PlasmaCircles::ConcentricSmooth;         break;
		case CONCENTRIC_SHARPEDGE_CIRCLES:  generator_ = (PFN_PIXELGEN)&PlasmaCircles::ConcentricSharpenEdges;   break;
		case HORIZONTAL_GRADIENT:           generator_ = (PFN_PIXELGEN)&PlasmaCircles::HorizontalGradient;       break;
		case VERTICAL_GRADIENT:             generator_ = (PFN_PIXELGEN)&PlasmaCircles::VerticalGradient;         break;
		case DIAGONAL_GRADIENT:             generator_ = (PFN_PIXELGEN)&PlasmaCircles::DiagonalGradient;         break;
		}
	}

	void PlasmaCircles::SelectTexture( DXTexture & Tex, UINT nLevel )
	{
		SoftwareGenerator::SelectTexture( Tex, nLevel );
		params_.width	= texDesc_.Width;
		params_.height	= texDesc_.Height;
		params_.centerx	= params_.width / 2;
		params_.centery	= params_.height / 2;
	}

	bool PlasmaCircles::OnBegin()
	{
		return true;
	}

	void PlasmaCircles::Write8( BYTE * pBits, UINT x, UINT y, UINT nLine, UINT nIndex )
	{
		BYTE Pixel = (BYTE)((*this).*(generator_))( pBits, x, y );
		pBits[ nIndex ] = Pixel;
	}

	void PlasmaCircles::Defaults()
	{
		params_.radius		= 1.2;
		params_.consistence	= 90.0;
		params_.divisor		= 16.0;
		params_.start		= 16.0;
		params_.centerx		= params_.width / 2;
		params_.centery		= params_.height / 2;
	}

	SoftwareGenerator::PIXEL PlasmaCircles::HorizontalGradient(BYTE * pBits, UINT x, UINT y)
	{
		return (x/params_.width)*255;
	}

	SoftwareGenerator::PIXEL PlasmaCircles::VerticalGradient(BYTE * pBits, UINT x, UINT y)
	{
		return (y/params_.height)*255;
	}

	SoftwareGenerator::PIXEL PlasmaCircles::DiagonalGradient(BYTE * pBits, UINT x, UINT y)
	{
		return (x/params_.width)*127 + (y/params_.height)*127;
	}

	SoftwareGenerator::PIXEL PlasmaCircles::ConcentricSharpenEdges(BYTE *pBits, UINT x, UINT y)
	{
		BYTE Pixel = ((params_.start + sqrt(params_.start + (params_.centery-y)*(params_.centery-y) 
			+ (params_.centerx-x)*(params_.centerx-x)) - 4) * params_.radius);
		return Pixel;
	}

	SoftwareGenerator::PIXEL PlasmaCircles::ConcentricSmooth(BYTE * pBits, UINT x, UINT y)
	{
		double dCirc = (sqrt(params_.start + (params_.centery-y)*(params_.centery-y) 
			+ (params_.centerx-x)*(params_.centerx-x)) - 4 );
		BYTE rv = ((sin(dCirc/params_.divisor) + 1 ) * params_.consistence);
		return rv;
	}


	void PlasmaBlender::ZeroBalls()
	{
		for (int i=0; i<MAX_BALL; i++)
		{
			ball_[i].a = 0.0f;
			ball_[i].b = 0.0f;
			ball_[i].x = 0;
			ball_[i].y = 0;
		}

		roll_ = 0;
	}

	void PlasmaBlender::DefaultPalette( bool bRed, bool bGreen, bool bBlue, UINT nAlpha )
	{
		for (int i=0; i<256; i++)
		{
			UINT nColor = nAlpha << 24;
			int C = i;
			if (C > 127) C = 255 - C;
			C *= 2;
			
			if ( bRed )		nColor |= C << 16;
			if ( bGreen )	nColor |= C << 8;
			if ( bBlue )	nColor |= C;

			palette_[ i ] = nColor;
		}
	}

	void PlasmaBlender::SetAlpha(UINT nAlpha)
	{
		nAlpha <<= 24;
		for (int i=0; i<256; i++)
		{
			UINT nColor = palette_[ i ] & 0x00ffffff;
			palette_[ i ] = nColor | nAlpha;
		}
	}

	PlasmaBlender::PlasmaBlender()
		: spillover_( true )
	{
		srcTex_[0] = NULL;
		srcTex_[1] = NULL;
		ZeroBalls();
	}

	PlasmaBlender::PlasmaBlender(DXTexture & Target, DXTexture & Tex1, DXTexture & Tex2, UINT nLevel)
		: spillover_( true )
	{
		SelectTexture( Target, Tex1, Tex2, nLevel );
		ZeroBalls();
	}

	void PlasmaBlender::SelectTexture( DXTexture & Target, DXTexture & Tex1, DXTexture & Tex2, UINT nLevel )
	{
		SoftwareGenerator::SelectTexture( Target, nLevel );
		srcTex_[0] = Tex1.GetObject();
		srcTex_[1] = Tex2.GetObject();
		centerX_	= texDesc_.Width/2;
		centerY_	= texDesc_.Height/2;
	}

	void PlasmaBlender::OnEnd()
	{
		for ( int i=0; i<MAX_TEX; i++ )
		{
			srcTex_[i]->UnlockRect(texLevel_);
		}
	}

	bool PlasmaBlender::OnBegin()
	{
		D3DLOCKED_RECT	 sLock[ MAX_TEX ];
		D3DSURFACE_DESC	 D3DSD[ MAX_TEX ];

		for ( int i=0; i<MAX_TEX; i++ )
		{
			if ( !srcTex_[i] ) return false;
			if ( FAILED(srcTex_[i]->GetLevelDesc(texLevel_, &D3DSD[i] )) ) return false;
			RECT rc = { 0, 0, D3DSD[i].Width, D3DSD[i].Height };
			if ( FAILED(srcTex_[i]->LockRect(texLevel_, &sLock[i], &rc, 0 )) ) return false;
		}

		bufA_		= (BYTE*)sLock[0].pBits;
		bufB_		= (BYTE*)sLock[1].pBits;
		pitchA_		= sLock[0].Pitch;
		pitchB_		= sLock[1].Pitch;

		ball_[0].Inc( 0.3/6, -0.2/6 );
		ball_[1].Inc( 0.085/6, -0.1/6 );
		ball_[2].Inc( 0.4/6, -0.15/6 );
		ball_[3].Inc( 0.35/6, -0.05/6 );

		ball_[0].x = centerX_ + centerX_ * cos(ball_[0].a);
		ball_[0].y = centerY_ + centerY_ * sin(ball_[0].b);

		ball_[1].x = centerX_ + centerX_ * sin(ball_[1].a);
		ball_[1].y = centerY_ + centerY_ * cos(ball_[1].b);

		ball_[2].x = centerX_ + centerX_ * cos(ball_[2].a);
		ball_[2].y = centerY_ + centerY_ * sin(ball_[2].b);

		ball_[3].x = centerX_ + centerX_ * cos(ball_[3].a);
		ball_[3].y = centerY_ + centerY_ * sin(ball_[3].b);

		roll_ += 2;

		int nHeight = texDesc_.Height;
		int nWidth  = texDesc_.Width;

		for ( int i=0; i<MAX_BALL; i++ )
		{
			while ( ball_[i].x > nWidth ) 
				ball_[i].x -= nWidth;

			while ( ball_[i].y > nHeight ) 
				ball_[i].y -= nHeight;
		}

		if ( spillover_ )
		{
			static double 
				  r = 1.0/6.0*M_PI
				, g = 3.0/6.0*M_PI
				, b = 5.0/6.0*M_PI;

			for (int i=0; i<256; i++)
			{
				double u = 2*M_PI/256*i;
				#define	cosine_col(u, a) ( cos((u)+(a)) + 1 ) * 127;
				rgb_[ i ][ 0 ] = cosine_col(u, r);
				rgb_[ i ][ 1 ] = cosine_col(u, g);
				rgb_[ i ][ 2 ] = cosine_col(u, b);
			}

			r += 0.05;
			g -= 0.05;
			b += 0.1;
		}

		return true;
	}

	BYTE PlasmaBlender::BlendedPixel( UINT j, UINT i )
	{
		BYTE Pixel =	bufA_	[ pitchA_ * ( i + ball_[0].y ) + j + ball_[0].x ]  + roll_
					+	bufB_	[ pitchB_ * ( i + ball_[1].y ) + j + ball_[1].x ]
					+	bufB_	[ pitchB_ * ( i + ball_[2].y ) + j + ball_[2].x ]
					+	bufB_	[ pitchB_ * ( i + ball_[3].y ) + j + ball_[3].x ]
					;
		return Pixel;
	}

	void PlasmaBlender::Write8( BYTE * pBits, UINT x, UINT y, UINT nLine, UINT nIndex )
	{
		BYTE Pixel = BlendedPixel( x, y );
		if (Pixel > 128) Pixel = 255 - Pixel;
		pBits[ nIndex ] = Pixel;
	}

	void PlasmaBlender::Write8888( UINT * pBits, UINT x, UINT y, UINT nLine, UINT nIndex )
	{
		BYTE Pixel = BlendedPixel( x, y );
		pBits[ nIndex ] = palette_[ Pixel ];
	}
}
