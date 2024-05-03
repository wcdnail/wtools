#pragma once

#include "Texture.h"
#include "Plasma.h"

namespace TexGen
{
    class PixelGenerator
    {
    public:
        typedef UINT PIXEL;
        typedef PIXEL (PixelGenerator::*PFN_PIXELGEN)(BYTE*, UINT, UINT);
        typedef void (PixelGenerator::*PFN_PIXELWRITE)(void*, UINT, UINT, UINT, UINT);

        PixelGenerator()
            : generator_(NULL) 
            , writer_(NULL)
            , bytesPerPixel_(0)
        {}

        virtual ~PixelGenerator() {}

        virtual bool OnBegin();
        virtual void OnEnd();
        virtual void Write8(BYTE * pBits, UINT x, UINT y, UINT nLine, UINT nIndex);
        virtual void Write565(WORD * pBits, UINT x, UINT y, UINT nLine, UINT nIndex);
        virtual void Write888(UINT * pBits, UINT x, UINT y, UINT nLine, UINT nIndex);
        virtual void Write8888(UINT * pBits, UINT x, UINT y, UINT nLine, UINT nIndex);

    protected:
        void wrap8(void *, UINT, UINT, UINT, UINT);
        void wrap565(void *, UINT, UINT, UINT, UINT);
        void wrap888(void *, UINT, UINT, UINT, UINT);
        void wrap8888(void *, UINT, UINT, UINT, UINT);

    protected:
        PFN_PIXELGEN    generator_;
        PFN_PIXELWRITE  writer_;
        UINT            bytesPerPixel_;

        void SelectPixelFormat(D3DFORMAT);
    };

    class SoftwareGenerator: public PixelGenerator
	{
	public:
		SoftwareGenerator()
            : dxTex_(NULL)
            , texLevel_(0)
        {}

		SoftwareGenerator(DXTexture const& Tex, UINT nLevel)
            : dxTex_(NULL)
            , texLevel_(0)
        {
            SelectTexture(Tex, nLevel);
        }

		void SelectTexture(DXTexture const& Tex, UINT nLevel = 0);
		virtual void Generate();

	protected:
		IDirect3DTexture9 * dxTex_;
		UINT				texLevel_;
		D3DLOCKED_RECT		rcLock_;
		D3DSURFACE_DESC		texDesc_;
	};

	class PlasmaCircles: public SoftwareGenerator
	{
	public:
		PlasmaCircles();
		PlasmaCircles(DXTexture & Tex, UINT nLevel = 0);
		void Defaults();
		void SelectAlgorithm(int);
		void SelectTexture(DXTexture & Tex, UINT nLevel = 0);
		int GetAlgorithm() const { return params_.algo; }

        PlasmaParams& GetParams() { return params_; }
		
	private:
		bool OnBegin();
		void Write8	(BYTE *pBits, UINT x, UINT y, UINT nLine, UINT nIndex);

		PIXEL HorizontalGradient(BYTE *pBits, UINT x, UINT y);
		PIXEL VerticalGradient(BYTE *pBits, UINT x, UINT y);
		PIXEL DiagonalGradient(BYTE *pBits, UINT x, UINT y);
		PIXEL ConcentricSharpenEdges(BYTE * pBits, UINT x, UINT y);
		PIXEL ConcentricSmooth(BYTE *pBits, UINT x, UINT y);

        PlasmaParams params_;
	};

	class PlasmaBlender: public SoftwareGenerator
	{
	public:
		PlasmaBlender( DXTexture & Target, DXTexture & Tex1, DXTexture & Tex2, UINT nLevel = 0 );
		PlasmaBlender();
		void SelectTexture( DXTexture & Target, DXTexture & Tex1, DXTexture & Tex2, UINT nLevel = 0 );
		void DefaultPalette( bool = false, bool = false, bool = true, UINT = 0xff );
		void DefaultPaletteBlue( UINT nAlpha = 0xff ) { DefaultPalette( false, false, true, nAlpha ); }
		void DefaultPaletteGreen( UINT nAlpha = 0xff ) { DefaultPalette( false, true, false, nAlpha ); }
		void DefaultPaletteRed( UINT nAlpha = 0xff ) { DefaultPalette( true, false, false, nAlpha ); }
		void SpilloverOff() { spillover_ = false; }
		void SpilloverOn() { spillover_ = true; }
		bool& Spillover() { return spillover_; }
		void SetAlpha ( UINT nAlpha = 0xff );
	private:
		bool OnBegin	();
		void OnEnd		();
		void Write8		( BYTE *, UINT, UINT, UINT, UINT );
		void Write8888	( UINT *, UINT, UINT, UINT, UINT );
		void ZeroBalls	();
		BYTE BlendedPixel( UINT, UINT );

		enum { MAX_TEX = 2 };
		enum { MAX_BALL = 4 };

		struct Ball
		{
			double	a, b;
			int		x, y;
			void Inc(double e, double f)    { a += e; b += f; }
			void Set(int e, int f)          { x = e; y = f; }
		};

		IDirect3DTexture9 * srcTex_         [MAX_TEX];
		Ball                ball_           [MAX_BALL];
		union {	UINT        palette_        [ 256 ];
				BYTE        rgb_            [ 256 ][4]; };
		BYTE *              bufA_;
		BYTE *              bufB_;
		UINT                pitchA_;
		UINT                pitchB_;
		UINT                centerX_;
		UINT                centerY_;
		BYTE                roll_;
		bool                spillover_;
	};
}

