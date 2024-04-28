#pragma once

namespace quickdialog
{
  enum check_state 
  { 
    csChecked = BST_CHECKED, 
    csUnchecked = BST_UNCHECKED, 
    csIndeterminate = BST_INDETERMINATE
  };

  template<typename _Chr, class _Base>
  class button_base :
    public subclassed_control<_Chr, _Base>
  {
    private:
      check_state _initialstate;
    public:
      button_base( DWORD style, DWORD exstyle, std::basic_string<_Chr> windowtext, check_state state  )
        : subclassed_control<_Chr, _Base>( style, exstyle, LITERAL(_Chr, "BUTTON"), windowtext),
          _initialstate( state )
      {
      
      }

      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd) { return; }
        subclassed_control<_Chr, _Base>::init(wnd);
        wnd.reflectcommand(this->_hwnd);
        putstate(_initialstate);
      }

      LRESULT wndproc(UINT msg, WPARAM wparam, LPARAM lparam)
      {
        switch (msg)
        {
          case WM_COMMAND:
            if (HIWORD(wparam) == BN_CLICKED && reinterpret_cast<HWND>(lparam) == this->_hwnd)
            { OnClick(); }
            if (reinterpret_cast<HWND>(lparam) == this->_hwnd)
            { return 0; }
            break;
          case WM_ERASEBKGND:
            return 0;
        }
        return subclassed_control<_Chr, _Base>::wndproc(msg, wparam, lparam);
      }

      virtual void OnClick()
      { }

      check_state getstate()
      { return static_cast<check_state>(IsWindow(this->_hwnd) ? win::sendmessage<_Chr>(this->_hwnd, BM_GETCHECK, 0, 0) : _initialstate); }

      void putstate(check_state state)
      { 
        if (IsWindow(this->_hwnd))
        { win::sendmessage<_Chr>(this->_hwnd, BM_SETCHECK, static_cast<WPARAM>(state), 0); }
        else
        { _initialstate = state; }
      }

      ~button_base()
      {
      }
  };
}