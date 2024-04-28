#pragma once

namespace quickdialog
{
  enum edit_styles
  { 
    esNone = 0,
    esPassword = ES_PASSWORD,
    esLowercase = ES_LOWERCASE,
    esUppercase = ES_UPPERCASE,
    esMultiline = ES_MULTILINE | ES_AUTOVSCROLL,
    esReadOnly = ES_READONLY,
    esNumber = ES_NUMBER,
    esWantReturn = ES_WANTRETURN,
    esWordwrap = 0x8000 // Must be higher than largest editbox style
  };

  template<typename _Chr, class _Base>
  class edit_base : public subclassed_control<_Chr, _Base>
  {
    protected:
      bool _wordwrap;
      unsigned int _minwidth, _minheight;
      std::basic_string<_Chr> &_text;
      std::basic_string<_Chr> _cuebanner;
      std::tr1::function<void(abstractcontrol *sender, std::basic_string<_Chr>)> _changefunctor;

      virtual void ondestroy()
      {
        win::retrievewindowtext(this->_hwnd, _text);
        subclassed_control<_Chr, _Base>::ondestroy();
      }

    public:
      edit_base(std::basic_string<_Chr> &text, const _Chr *windowclass, unsigned style, unsigned int exstyle, bool wordwrap, unsigned int minwidth, unsigned int minheight, const std::basic_string<_Chr> &cuebanner)
        : subclassed_control<_Chr, _Base>((style & ~esWordwrap) | ES_AUTOHSCROLL | WS_TABSTOP, exstyle, windowclass, text),
          _text(text),
          _cuebanner(cuebanner),
          _wordwrap(wordwrap),
          _minwidth(minwidth),
          _minheight(minheight)
      { }

      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd)
        { return; }
        subclassed_control<_Chr, _Base>::init(wnd);
        if (!_cuebanner.empty())
        { win::sendmessage<_Chr>(this->_hwnd, EM_SETCUEBANNER, FALSE, (LPARAM)string::convert<_Chr, wchar_t>(_cuebanner).c_str()); }
        win::sendmessage<_Chr>(this->_hwnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(_text.c_str()));
      }

      SIZE requestsize()
      {
        RECT rect = {};
        AdjustWindowRectEx(&rect, this->_style, false, this->_exstyle);
        SIZE result = { rect.right - rect.left, rect.bottom - rect.top };
        DWORD style = GetWindowLong(this->_hwnd, GWL_STYLE);
        DWORD exstyle = GetWindowLong(this->_hwnd, GWL_EXSTYLE);
        if (style & WS_BORDER)
        { result.cx += 2; result.cy += 5; }
        else if (exstyle & WS_EX_CLIENTEDGE)
        { result.cy += 3;}
        //style & ES_MULTILINE, _wordwrap, _minwidth
        SIZE s = this->sizeofwindowtext();
        result.cx += max(s.cx, static_cast<int>(_minwidth));
        result.cy += max(s.cy, static_cast<int>(_minheight));

        if (style & WS_VSCROLL)
        { result.cx += GetSystemMetrics(SM_CXVSCROLL); }
        if (style & WS_HSCROLL)
        { result.cy += GetSystemMetrics(SM_CYHSCROLL); }
        return result;
      }
  };

  template<typename _Chr>
  class basic_edit : public edit_base<_Chr, basic_edit<_Chr> >
  {
    public:
      basic_edit(std::basic_string<_Chr> &text, int style = esNone, unsigned int minwidth = 50, unsigned int minheight = 0, const std::basic_string<_Chr> &cuebanner = std::basic_string<_Chr>())
        : edit_base<_Chr, basic_edit<_Chr> >(text, LITERAL(_Chr, "EDIT"), (style & ~esWordwrap) | ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE, (style & esWordwrap) != 0, minwidth, minheight, cuebanner)
      {
      }
  };

  CTRL_TYPEDEFS(edit)
}