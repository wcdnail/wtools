#pragma once

namespace quickdialog
{
  template<typename _Chr>
  class basic_check_box : public button_base<_Chr, basic_check_box<_Chr> >
  {
    private:
      std::tr1::function<void (abstractcontrol * sender, bool checked)> _clickfunctor;
      bool &_checkstate;
    public:
      basic_check_box(std::basic_string<_Chr> text, bool &checked)
        : button_base<_Chr, basic_check_box<_Chr> >(BS_AUTOCHECKBOX | WS_TABSTOP, 0, text, checked ? csChecked : csUnchecked),
          _checkstate(checked)
      { }

      template<class _ClickHandler>
      basic_check_box(std::basic_string<_Chr> text, bool &checkstate, _ClickHandler clickhandler)
        : button_base<_Chr, basic_check_box<_Chr> >(BS_AUTOCHECKBOX | WS_TABSTOP, 0, text, checkstate ? csChecked : csUnchecked),
          _checkstate(checkstate),
          _clickfunctor(clickhandler)
      { }

      virtual void init(abstractwindow &wnd)
      {
        button_base<_Chr, basic_check_box<_Chr> >::init(wnd);
        this->putstate(_checkstate ? csChecked : csUnchecked);
      }

      virtual void OnClick()
      { 
        check_state state = this->getstate();
        _checkstate = (state == csChecked);
        if (_clickfunctor)
        { _clickfunctor(this, _checkstate); }
      }

      const RECT &getmargins()
      {
        static RECT r = { 2, 2, 2, 2 };
        return r;
      }

      virtual SIZE requestsize()
      {
        SIZE s = this->sizeofwindowtext();
        s.cx += 25;
        s.cy += 5;
        return s;
      }
  };

  template<typename _Chr>
  class basic_tri_state_check_box : public button_base<_Chr, basic_tri_state_check_box<_Chr> >
  {
    private:
      std::tr1::function<void (abstractcontrol *sender, check_state checked)> _clickfunctor;
      check_state &_checkstate;
    public:
      basic_tri_state_check_box(std::basic_string<_Chr> text, check_state &checkstate)
        : button_base<_Chr, basic_tri_state_check_box<_Chr> >(BS_AUTO3STATE | WS_TABSTOP, 0, text, checkstate),
          _checkstate(checkstate)
      { }

      template<class _ClickHandler>
      basic_tri_state_check_box(std::basic_string<_Chr> text, check_state &checkstate, _ClickHandler clickhandler)
        : button_base<_Chr, basic_tri_state_check_box<_Chr> >(BS_AUTO3STATE | WS_TABSTOP, 0, text, checkstate),
          _checkstate(checkstate),
          _clickfunctor(clickhandler)
      { }

      virtual void OnClick()
      { 
        _checkstate = this->getstate();
        if (_clickfunctor)
        { _clickfunctor(this, _checkstate); }
      }

      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd) { return; }
        button_base<_Chr, basic_tri_state_check_box<_Chr> >::init(wnd);
        this->putstate(_checkstate);
      }

      const RECT &getmargins()
      {
        static RECT r = { 2, 2, 2, 2 };
        return r;
      }

      virtual SIZE requestsize()
      {
        SIZE s = this->sizeofwindowtext();
        s.cx += 25;
        s.cy += 5;
        return s;
      }
  };

  CTRL_TYPEDEFS(check_box)
  CTRL_TYPEDEFS(tri_state_check_box)
}