#pragma once

namespace quickdialog
{    
  enum font_style { fsNormal = 0, fsBold = 1, fsItalic = 2, fsUnderline = 4, fsStrikethrough = 8 };

  template<class _Chr>
  class basic_paragraph : public subclassed_control<_Chr, basic_paragraph<_Chr> >
  {
  private:
    COLORREF _colour;
    shared_handle<HFONT> _font;

    virtual const RECT &getmargins()
    {
      static RECT margins = { 4, 2, 4, 2 };
      return margins;
    }
    
    shared_handle<HFONT> setfont(const std::basic_string<_Chr> &fontname, int pointsize, font_style fontstyle)
    {
      if (fontname.size())
      {
        //_font = win::createfont(-13,0,0,0,FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, LITERAL(_Chr, "MS Shell Dlg 2"));
        return shared_handle<HFONT>(win::createfont(pointsize, 0, 0, 0, 
                                (fontstyle & fsBold) ? FW_BOLD : FW_NORMAL, 
                                ((fontstyle & fsItalic) != 0) ? TRUE : FALSE,
                                ((fontstyle & fsUnderline) != 0) ? TRUE : FALSE,
                                ((fontstyle & fsStrikethrough) != 0) ? TRUE : FALSE,
                                DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS,
                                DEFAULT_QUALITY,
                                DEFAULT_PITCH,
                                fontname), delete_object());
      }
      else
      { return shared_handle<HFONT>(NULL, null_deleter()); }
    }

  public:
    basic_paragraph(const std::basic_string<_Chr> &str, const std::basic_string<_Chr> &fontname = std::basic_string<_Chr>(LITERAL(_Chr, "")), int pointsize = 8, font_style style = fsNormal, COLORREF colour = -1)
      : subclassed_control<_Chr, basic_paragraph<_Chr> >(WS_CLIPSIBLINGS, 0, LITERAL(_Chr, "STATIC"), str),
        _colour(colour),
        _font(setfont(fontname, pointsize, style))
    { }

    basic_paragraph(const _Chr *str, const std::basic_string<_Chr> &fontname = LITERAL(_Chr, ""), int pointsize = 8, font_style style = fsNormal, COLORREF colour = -1)
      : subclassed_control<_Chr, basic_paragraph<_Chr> >(WS_CLIPSIBLINGS, 0, LITERAL(_Chr, "STATIC"), str),
        _colour(colour),
        _font(setfont(fontname, pointsize, style))
    { }

    virtual LRESULT wndproc(UINT msg, WPARAM wparam, LPARAM lparam)
    {
      PAINTSTRUCT ps;
      RECT r;
      switch (msg)
      {
        case WM_PAINT:
          BeginPaint(this->_hwnd, &ps);
          GetClientRect(this->_hwnd, &r);
          SetBkMode(ps.hdc, TRANSPARENT);
          SetTextColor(ps.hdc, _colour == -1 ? GetSysColor(COLOR_WINDOWTEXT) : _colour);
          HGDIOBJ oldfont = SelectObject(ps.hdc, reinterpret_cast<HFONT>(win::sendmessage<_Chr>(this->_hwnd, WM_GETFONT, 0, 0)));
          win::drawtext(ps.hdc, this->_windowtext, r, 0);
          SelectObject(ps.hdc, oldfont);
          EndPaint(this->_hwnd, &ps);
          return 0;
      }
      return subclassed_control<_Chr, basic_paragraph<_Chr> >::wndproc(msg, wparam, lparam);
    }

    virtual void init(abstractwindow &wnd)
    {
      if (this->_hwnd)
      { return; }
      subclassed_control<_Chr, basic_paragraph<_Chr> >::init(wnd);
      wnd.reflectnotify(this->_hwnd);
      if (_font.get())
      { win::sendmessage<_Chr>(this->_hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(_font.get()), FALSE); }
    }

    virtual SIZE requestsize()
    { return this->sizeofwindowtext(); }
  };

  CTRL_TYPEDEFS(paragraph)
}