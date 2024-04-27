#pragma once

namespace quickdialog
{
  template<typename _Chr>
  class basic_button : public button_base<_Chr, basic_button<_Chr> >
  {
    private:
      dialogresult _returnvalue;
      std::tr1::function<void (abstractcontrol *sender)> _clickfunctor;
    public:
      basic_button(std::basic_string<_Chr> text, dialogresult returnvalue = qdOK, bool defaultButton = false)
        : button_base<_Chr, basic_button<_Chr> >((defaultButton ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP, 0, text, csUnchecked),
          _returnvalue(returnvalue)
      { }

      template<class _ClickHandler>
      basic_button(std::basic_string<_Chr> text, _ClickHandler clickhandler, dialogresult returnvalue = qdNone,  bool defaultButton = false)
        : button_base<_Chr, basic_button<_Chr> >((defaultButton ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP, 0, text, csUnchecked),
          _clickfunctor(clickhandler),
          _returnvalue(returnvalue)
      { }

      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd)
        { return; }
        button_base<_Chr, basic_button<_Chr> >::init(wnd);
        if (this->_style & BS_DEFPUSHBUTTON)
        { this->getparentdialog().setdefaultbutton(this->_hwnd); }
        if (_returnvalue == qdCancel)
        { this->getparentdialog().setcancelbutton(this->_hwnd); }
      }

      virtual void OnClick()
      { 
        if (_clickfunctor)
        { _clickfunctor(this); }
        
        if (_returnvalue != qdNone)
        { 
          this->getparentdialog()._returnvalue = _returnvalue; 
          win::sendmessage<_Chr>(this->getparentdialog().gethwnd(), WM_CLOSE, 0, 0);
        }
      }

      const RECT &getmargins()
      {
        static RECT r = { 2, 0, 2, 0 };
        return r;
      }

      virtual SIZE requestsize()
      {
        SIZE s = button_base<_Chr, basic_button<_Chr> >::sizeofwindowtext();
        s.cx += 24;
        s.cy += 10;
        return s;
      }
  };

  CTRL_TYPEDEFS(button)
}