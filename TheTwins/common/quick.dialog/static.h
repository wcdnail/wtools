//
// Author: Nikonov M. 2012.
// 

#pragma once

namespace quickdialog
{
  enum static_styles
  {
      ssNone = 0,
      ssSunken = SS_SUNKEN,
      ssEndEllipsis = SS_ENDELLIPSIS,
      ssPathEllipsis = SS_PATHELLIPSIS,
      ssBorder = WS_BORDER,
      ssCenter = SS_CENTER,
      ssVCenter = SS_CENTERIMAGE,
      ssClientEdge = WS_EX_CLIENTEDGE,
  };

  template<typename _Chr, class _Base>
  class static_base : public subclassed_control<_Chr, _Base>
  {
    public:
      typedef std::basic_string<_Chr> string_type;
      typedef subclassed_control<_Chr, _Base> super;

      static_base(string_type const& line, unsigned style = ssNone, unsigned exstyle = ssNone, int cx = -1, int cy = -1)
          : super(style, exstyle, LITERAL(_Chr, "STATIC"), line),
          cx_(cx),
          cy_(cy),
          text_(NULL)
      {}

      static_base(WTL::CStatic& text, string_type const& line, unsigned style = ssNone, unsigned exstyle = ssNone, int cx = -1, int cy = -1)
        : super(style, exstyle, LITERAL(_Chr, "STATIC"), line),
          cx_(cx),
          cy_(cy),
          text_(&text)
      {}

      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd)
            return;

        subclassed_control<_Chr, _Base>::init(wnd);
        if (NULL != text_)
            text_->Attach(this->_hwnd);
      }

      SIZE requestsize()
      {
        SIZE ts = this->sizeofwindowtext();
        SIZE rv = { (-1 == cx_ ? ts.cx : cx_), (-1 == cy_ ? ts.cy : cy_) };
        return rv;
      }

    protected:
      int cx_;
      int cy_;
      WTL::CStatic* text_;
    
      virtual void ondestroy()
      {
          subclassed_control<_Chr, _Base>::ondestroy();
      }
  };

  template<typename _Chr>
  class basic_text : public static_base<_Chr, basic_text<_Chr> >
  {
    public:
      typedef static_base<_Chr, basic_text<_Chr> > super;

      basic_text(WTL::CStatic& text, string_type const& line, int cx = 120, int cy = -1, unsigned style = ssNone, unsigned exstyle = ssNone)
        : super(text, line, style, exstyle, cx, cy)
      {}

      basic_text(string_type const& line, int cx = 120, int cy = -1, unsigned style = ssNone, unsigned exstyle = ssNone)
          : super(line, style, exstyle, cx, cy)
      {}
  };

  CTRL_TYPEDEFS(text)
}