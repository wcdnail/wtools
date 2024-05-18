#include "stdafx.h"
#include "colorselector.h"
#include "SpectrumWnd.h"
#include <cmath>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

#ifdef _DEBUG
#  define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////
//
// This class paints a spectrum of colors all having a constant luminance
// value, while varying the luminance and saturation values.

// The spectrum fills the entire window, except for a thin one-pixel border
// that surrounds it and is painted in the COLOR_BTNFACE color.  The border 
// allows display of an outlined tiny rectangle to signify which color is 
// closest to a target color

// The constant luminance value is stored in m_iLum
// The target color is m_crTargetColor
// Both are assumed to have been set by an outside source

// If the user drags or clicks in the window, the position of the outline
// is changed, and the window sends its parent a private message whose value
// is UWM_SPECTRUMWINDOWEVENT and which must be defined in the project
// somewhere (such as NameOfApp.h).  The WPARAM of the message has a value
// of SWE_NEWFOCUSCOLOR.  In response, the parent should set the new target
// color (i.e., m_crTargetColor) and/or the luminance (i.e., m_iLum) and then
// invalidate this window or otherwise cause it to re-paint itself.  To assist
// the parent in deciding how to set the target color, the window stores
// the value of the clicked-on color in m_crCurrentFocusColor


const int CSpectrumWnd::m_cbCells = 24;


/////////////////////////////////////////////////////////////////////////////
// CSpectrumWnd

CSpectrumWnd::CSpectrumWnd()
{
    m_iLum = 0;
    m_bIsDragging = FALSE;
    
    // allocate memory to store the RGB values of each tiny
    // rectangle in the spectrum's window
    
    m_crArray = new COLORREF[m_cbCells*m_cbCells];
}

CSpectrumWnd::~CSpectrumWnd()
{
    delete[] m_crArray;
}


BEGIN_MESSAGE_MAP(CSpectrumWnd, CWnd)
//{{AFX_MSG_MAP(CSpectrumWnd)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSpectrumWnd message handlers


int CSpectrumWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    // adjust size of client rectangle to a nice multiple of
    // the number of cells being displayed

    // adjust rect to be a multiple of the number of cells contained in the CSpectrum,
    // plus 2 pixels (to allow for outside-the-square drawing of the selection outline)
    
    CRect rc;
    GetClientRect( &rc );

    int ccx, ccy;
    ccx = (rc.Width()+m_cbCells/2)/m_cbCells;   // allow for modest expansion of window's size
    ccy = (rc.Height()+m_cbCells/2)/m_cbCells;
    
    rc.right = ccx*m_cbCells + 2;
    rc.bottom = ccy*m_cbCells + 2;

    this->SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(),
        SWP_NOMOVE | SWP_NOZORDER );


    return 0;
}


void CSpectrumWnd::OnPaint() 
{
    CPaintDC dc(this); // device context for painting
    
    CRect rc;
    GetClientRect( &rc );
    
    // our window is framed by a 1-pixel wide blank border, to accommodate
    // the outside-the-box sized focus outline for the current color
    // so we need to erase one pixel wide in the border, or else we
    // will leave remnants of the outline
    
    CBrush brGray( ::GetSysColor( COLOR_BTNFACE ) );
    dc.FrameRect( &rc, &brGray );
    
    int dx = rc.Width()/m_cbCells;
    int dy = rc.Height()/m_cbCells;
    
    int hue, lum, sat;
/// int dr, dg, db;     // delta errors in RGB space (not needed, see below)
    int dclr, dclrmax;  // color difference errors
    int dH, dL, dS;     // deltas in HLS coords
    WORD HH, LL, SS;    // m_crTargetColor in HLS coords
    dclrmax = SHRT_MAX; // big number
    float dColor = 240.0F/m_cbCells;
    CRect block(0,0,0,0);
    CSize offset(dy, dy);
    COLORREF cr;
    double TwoPiOver240 = 2.0 * 3.1415926 / 240.0;
    
    for ( int nDexY=0; nDexY<m_cbCells; nDexY++ )
    {
        // set the start location of the current drawing rectangle
        // offset by one pixel from window origin to allow drawing of focus outline rect
        
        block = CRect( 1, nDexY*dy+1, dx+1, (nDexY+1)*dy+1 );

        for ( int nDexX=0; nDexX<m_cbCells; nDexX++ )
        {
            hue = (int)dColor*nDexX;
            lum = m_iLum;
            sat = (int)(dColor*((float)nDexY+0.5F));
            
            cr = ::ColorHLSToRGB( hue, lum, sat );
            *(m_crArray+nDexY*m_cbCells+nDexX) = cr;
            dc.FillSolidRect( &block, cr );
            
/*
            // this computation of color difference is done is non-preferred
            // RGB coordinates, and has been replaced by a more perceptual
            // color difference calculation in HLS coordinates, below
            
            dr = GetRValue(m_crTargetColor) - GetRValue(cr);
            dg = GetGValue(m_crTargetColor) - GetGValue(cr);
            db = GetBValue(m_crTargetColor) - GetBValue(cr);
              
            dclr = (dr*dr)+(dg*dg)+(db*db);
*/
            
            // compute color difference in HLS space (which are cylindrical coords)
            // dc = s1^2 + s2^2 - 2*s1*s2*cos(h1-h2) + (l1-l2)^2
            
            ::ColorRGBToHLS( m_crTargetColor, &HH, &LL, &SS );
            
            dH = HH - hue;
            dL = LL - lum;
            dS = SS*SS + sat*sat;
            
            dclr = (int)((double)dS - 2.0*SS*sat*cos( TwoPiOver240*dH ) + (double)dL*dL);
            
            if (dclr<dclrmax)
            {
                dclrmax=dclr;
                m_rcFocusOutline = block;
                m_crCurrentFocusColor = cr;
            }
            
            block.OffsetRect( dx, 0 );
        }
    }
    
    // outline the rectangle that contains the target color
    
    m_rcFocusOutline.InflateRect(1,1);
    
    // if underlying colors are pale, use dark yellow outline for the target
    // otherwise, if underlying colors are dark enough, use a white brush
    
    COLORREF crOutline = ( m_iLum > 210 ) ? RGB( 200, 200, 100 ) : RGB(255,255,255);
    CBrush brush( crOutline );
    dc.FrameRect( &m_rcFocusOutline, &brush );
    
    
    // Do not call CWnd::OnPaint() for painting messages
}

void CSpectrumWnd::SetLum(int iLum)
{
    ASSERT( iLum>=0 );
    m_iLum = iLum;
}


void CSpectrumWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
    // check if user clicked inside or outside of current focus rectangle
    // if inside, start a drag operation
    // if outside, then re-set the current focus poition
    
    if ( m_rcFocusOutline.PtInRect( point ) )
    {
        m_bIsDragging = TRUE;
        SetCapture();
    }
    else
    {
        // determine which cell user clicked inside (don't forget about one-pixel offset)
        
        CPoint ccp = GetCellCoordinatesFromPoint( point );
        
        // set new focus color and tell parent to re-draw us
        
        m_crCurrentFocusColor = *(m_crArray + ccp.y*m_cbCells + ccp.x);
        GetParent()->SendMessage( UWM_SPECTRUMWINDOWEVENT, (WPARAM)SWE_NEWFOCUSCOLOR, 0L );
    }
    
    CWnd::OnLButtonDown(nFlags, point);
}

void CSpectrumWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
    
    if ( m_bIsDragging && nFlags==MK_LBUTTON )
    {   
        CPoint ccp = GetCellCoordinatesFromPoint( point );
            
        // set new focus color and tell parent to re-draw us
            
        m_crCurrentFocusColor = *(m_crArray + ccp.y*m_cbCells + ccp.x);
        GetParent()->SendMessage( UWM_SPECTRUMWINDOWEVENT, (WPARAM)SWE_NEWFOCUSCOLOR, 0L );
    }
    
    CWnd::OnMouseMove(nFlags, point);
}

void CSpectrumWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
    m_bIsDragging = FALSE;
    ReleaseCapture();
    
    CWnd::OnLButtonUp(nFlags, point);
}


#define RestrictToInclusiveRange( ii, min, max ) ((ii)<(min))?(min):( ((ii)>(max))?(max):(ii) )
CPoint CSpectrumWnd::GetCellCoordinatesFromPoint(CPoint point)
{
    // determine which cell user clicked inside (don't forget about one-pixel offset)
    
    CRect rc;
    GetClientRect( &rc );
    int dx = rc.Width()/m_cbCells;
    int dy = rc.Height()/m_cbCells;
    
    CPoint pt;
    pt.x = (point.x-1)/dx;
    pt.y = (point.y-1)/dy;

    pt.x = RestrictToInclusiveRange( pt.x, 0, m_cbCells-1 );
    pt.y = RestrictToInclusiveRange( pt.y, 0, m_cbCells-1 );

    ASSERT( pt.x + pt.y*m_cbCells < m_cbCells*m_cbCells );  // bounds check
    ASSERT( pt.x + pt.y*m_cbCells >= 0 );
    
    return pt;
}




// static const array of named colors, for the 
// foolish purpose of giving names to colors when copying them to the 
// clipboard
static const struct
{
    PCTSTR  name;
    COLORREF  cr;
}
NamedColors[]=
{
        _T("aliceblue"),               0x00FFF8F0,
        _T("antiquewhite"),            0x00D7EBFA,
        _T("aqua"),                    0x00FFFF00,
        _T("aquamarine"),              0x00D4FF7F,
        _T("azure"),                   0x00FFFFF0,
        _T("beige"),                   0x00DCF5F5,
        _T("bisque"),                  0x00C4E4FF,
        _T("black"),                   0x00000000,
        _T("blanchedalmond"),          0x00CDEBFF,
        _T("blue"),                    0x00FF0000,
        _T("blueviolet"),              0x00E22B8A,
        _T("brown"),                   0x002A2AA5,
        _T("burlywood"),               0x0087B8DE,
        _T("cadetblue"),               0x00A09E5F,
        _T("chartreuse"),              0x0000FF7F,
        _T("chocolate"),               0x001E69D2,
        _T("coral"),                   0x00507FFF,
        _T("cornflowerblue"),          0x00ED9564,
        _T("cornsilk"),                0x00DCF8FF,
        _T("crimson"),                 0x003C14DC,
        _T("cyan"),                    0x00FFFF00,
        _T("darkblue"),                0x008B0000,
        _T("darkcyan"),                0x008B8B00,
        _T("darkgoldenrod"),           0x000B86B8,
        _T("darkgray"),                0x00A9A9A9,
        _T("darkgreen"),               0x00006400,
        _T("darkkhaki"),               0x006BB7BD,
        _T("darkmagenta"),             0x008B008B,
        _T("darkolivegreen"),          0x002F6B55,
        _T("darkorange"),              0x00008CFF,
        _T("darkorchid"),              0x00CC3299,
        _T("darkred"),                 0x0000008B,
        _T("darksalmon"),              0x007A96E9,
        _T("darkseagreen"),            0x008BBC8F,
        _T("darkslateblue"),           0x008B3D48,
        _T("darkslategray"),           0x004F4F2F,
        _T("darkturquoise"),           0x00D1CE00,
        _T("darkviolet"),              0x00D30094,
        _T("deeppink"),                0x009314FF,
        _T("deepskyblue"),             0x00FFBF00,
        _T("dimgray"),                 0x00696969,
        _T("dodgerblue"),              0x00FF901E,
        _T("firebrick"),               0x002222B2,
        _T("floralwhite"),             0x00F0FAFF,
        _T("forestgreen"),             0x00228B22,
        _T("fuchsia"),                 0x00FF00FF,
        _T("gainsboro"),               0x00DCDCDC,
        _T("ghostwhite"),              0x00FFF8F8,
        _T("gold"),                    0x0000D7FF,
        _T("goldenrod"),               0x0020A5DA,
        _T("gray"),                    0x00808080,
        _T("green"),                   0x00008000,
        _T("greenyellow"),             0x002FFFAD,
        _T("honeydew"),                0x00F0FFF0,
        _T("hotpink"),                 0x00B469FF,
        _T("indianred"),               0x005C5CCD,
        _T("indigo"),                  0x0082004B,
        _T("ivory"),                   0x00F0FFFF,
        _T("khaki"),                   0x008CE6F0,
        _T("lavender"),                0x00FAE6E6,
        _T("lavenderblush"),           0x00F5F0FF,
        _T("lawngreen"),               0x0000FC7C,
        _T("lemonchiffon"),            0x00CDFAFF,
        _T("lightblue"),               0x00E6D8AD,
        _T("lightcoral"),              0x008080F0,
        _T("lightcyan"),               0x00FFFFE0,
        _T("lightgoldenrodyellow"),    0x00D2FAFA,
        _T("lightgray"),               0x00D3D3D3,
        _T("lightgreen"),              0x0090EE90,
        _T("lightpink"),               0x00C1B6FF,
        _T("lightsalmon"),             0x007AA0FF,
        _T("lightseagreen"),           0x00AAB220,
        _T("lightskyblue"),            0x00FACE87,
        _T("lightslategray"),          0x00998877,
        _T("lightsteelblue"),          0x00DEC4B0,
        _T("lightyellow"),             0x00E0FFFF,
        _T("lime"),                    0x0000FF00,
        _T("limegreen"),               0x0032CD32,
        _T("linen"),                   0x00E6F0FA,
        _T("magenta"),                 0x00FF00FF,
        _T("maroon"),                  0x00000080,
        _T("mediumaquamarine"),        0x00AACD66,
        _T("mediumblue"),              0x00CD0000,
        _T("mediumorchid"),            0x00D355BA,
        _T("mediumpurple"),            0x00DB7093,
        _T("mediumseagreen"),          0x0071B33C,
        _T("mediumslateblue"),         0x00EE687B,
        _T("mediumspringgreen"),       0x009AFA00,
        _T("mediumturquoise"),         0x00CCD148,
        _T("mediumvioletred"),         0x008515C7,
        _T("midnightblue"),            0x00701919,
        _T("mintcream"),               0x00FAFFF5,
        _T("mistyrose"),               0x00E1E4FF,
        _T("moccasin"),                0x00B5E4FF,
        _T("navajowhite"),             0x00ADDEFF,
        _T("navy"),                    0x00800000,
        _T("oldlace"),                 0x00E6F5FD,
        _T("olive"),                   0x00008080,
        _T("olivedrab"),               0x00238E6B,
        _T("orange"),                  0x0000A5FF,
        _T("orangered"),               0x000045FF,
        _T("orchid"),                  0x00D670DA,
        _T("palegoldenrod"),           0x00AAE8EE,
        _T("palegreen"),               0x0098FB98,
        _T("paleturquoise"),           0x00EEEEAF,
        _T("palevioletred"),           0x009370DB,
        _T("papayawhip"),              0x00D5EFFF,
        _T("peachpuff"),               0x00B9DAFF,
        _T("peru"),                    0x003F85CD,
        _T("pink"),                    0x00CBC0FF,
        _T("plum"),                    0x00DDA0DD,
        _T("powderblue"),              0x00E6E0B0,
        _T("purple"),                  0x00800080,
        _T("red"),                     0x000000FF,
        _T("rosybrown"),               0x008F8FBC,
        _T("royalblue"),               0x00E16941,
        _T("saddlebrown"),             0x0013458B,
        _T("salmon"),                  0x007280FA,
        _T("sandybrown"),              0x0060A4F4,
        _T("seagreen"),                0x00578B2E,
        _T("seashell"),                0x00EEF5FF,
        _T("sienna"),                  0x002D52A0,
        _T("silver"),                  0x00C0C0C0,
        _T("skyblue"),                 0x00EBCE87,
        _T("slateblue"),               0x00CD5A6A,
        _T("slategray"),               0x00908070,
        _T("snow"),                    0x00FAFAFF,
        _T("springgreen"),             0x007FFF00,
        _T("steelblue"),               0x00B48246,
        _T("tan"),                     0x008CB4D2,
        _T("teal"),                    0x00808000,
        _T("thistle"),                 0x00D8BFD8,
        _T("tomato"),                  0x004763FF,
        _T("turquoise"),               0x00D0E040,
        _T("violet"),                  0x00EE82EE,
        _T("wheat"),                   0x00B3DEF5,
        _T("white"),                   0x00FFFFFF,
        _T("whitesmoke"),              0x00F5F5F5,
        _T("yellow"),                  0x0000FFFF,
        _T("yellowgreen"),             0x0032CD9A 
};

CString CSpectrumWnd::GetNameOfClosestColor(COLORREF cr, COLORREF* crNamed /* = NULL */)
{
    // iterates through list of named colors to find the one with 
    // the smallest error in HLS coordinates
    
    int iMax = sizeof(NamedColors)/sizeof(NamedColors[0]);
    double twoPiOver240 = 2.0F * 3.1415926 / 240.0F;
    
    int dclr, dclrmax;  // delta errors
    int dH, dL, dS;     // deltas in HLS coords
    WORD tHH, tLL, tSS; // test color in HLS coords
    WORD iHH, iLL, iSS; // input color in HLS coords
    dclrmax = SHRT_MAX; // big number
    COLORREF crTest;
    CString strRet;
    strRet.Empty();
    
    ::ColorRGBToHLS( cr, &iHH, &iLL, &iSS );
    
    for ( int ii=0; ii<iMax; ii++ )
    {
        crTest = NamedColors[ii].cr;
        ::ColorRGBToHLS( crTest, &tHH, &tLL, &tSS );
        
        // compute color difference in HLS space (which are cylindrical coords)
        // dc = s1^2 + s2^2 - 2*s1*s2*cos(h1-h2) + (l1-l2)^2
        
        dH = iHH - tHH;
        dL = iLL - tLL;
        dS = iSS*iSS + tSS*tSS;
        
        dclr = (int)((double)dS - 2.0*iSS*tSS*cos( twoPiOver240*dH ) + (double)dL*dL);
        
        if (dclr<dclrmax)
        {
            dclrmax = dclr;
            strRet = NamedColors[ii].name;
            if ( crNamed )
                *crNamed = crTest;
        }
    }
    
    return strRet;
    
}

