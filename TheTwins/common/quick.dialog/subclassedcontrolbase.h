#pragma once

namespace quickdialog
{
  class atom_base
  {
    protected:
      static ATOM getpropertyatom()
      { 
        //TODO: better construct here - this needs to be deleted using GlobalDeleteAtom
        ATOM a = GlobalAddAtomW(L"ClassInstancePtr");
        return a;
      }
  };

  template<class _Chr, class _Base>
  class subclassed_control : public control<_Base, group>,
                             public atom_base
  {
    protected:
      DWORD _style, _exstyle;
      bool _enabled;
      std::basic_string<_Chr> _windowtext;
      const _Chr * _windowclass;
      WNDPROC _oldWndProc;
      abstractwindow *_wnd;
      std::tr1::unordered_set<HWND> _registeredfornotify, _registeredforcommand;
    
      virtual LRESULT wndproc(UINT msg, WPARAM wparam, LPARAM lparam)
      { 
        switch (msg)
        {
          case WM_NOTIFY:
            {
              if (_registeredfornotify.find(reinterpret_cast<NMHDR *>(lparam)->hwndFrom) != _registeredfornotify.end())
              { win::sendmessage<_Chr>(reinterpret_cast<NMHDR *>(lparam)->hwndFrom, msg, wparam, lparam);  }
              else if (reinterpret_cast<NMHDR *>(lparam)->hwndFrom == this->_hwnd)
              { return 0; } // Returns 0 in order to stop messages reflected to children bubbling back up to and causing an infinite loop.
            }
            break;
          case WM_COMMAND:
            {
              if (_registeredforcommand.find(reinterpret_cast<HWND>(lparam)) != _registeredforcommand.end())
              { win::sendmessage<_Chr>(reinterpret_cast<HWND>(lparam), msg, wparam, lparam);  }
              else if (reinterpret_cast<HWND>(lparam) == this->_hwnd)
              { return 0; } // Returns 0 in order to stop messages reflected to children bubbling back up to and causing an infinite loop.
            }
            break;
          case WM_DESTROY:
            ondestroy();
            break;
        }
        return win::callwindowproc<_Chr>(_oldWndProc, this->_hwnd, msg, wparam, lparam);
      }

      SIZE sizeofwindowtext()
      { return sizeofstr(_windowtext); }

      SIZE sizeofstr(const std::basic_string<_Chr> &_str)
      { 
        SIZE s;
        shared_handle<HDC> hdc(GetDC(this->_hwnd), release_dc(this->_hwnd));
        HGDIOBJ fnt = SelectObject(hdc.get(), reinterpret_cast<HFONT>(win::sendmessage<_Chr>(this->_hwnd, WM_GETFONT, 0, 0)));
        if (_str.empty())
        {
          s = win::getsizeoftext(hdc, std::basic_string<_Chr>(LITERAL(_Chr, "X"))); 
          s.cx = 0;
          return s;
        }
        else
        { s = win::getsizeoftext(hdc, _str); }
        SelectObject(hdc.get(), fnt);
        return s;
      }
    
      virtual void ondestroy()
      { this->_hwnd = NULL; }

    private:
      static LRESULT CALLBACK staticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
      {
        subclassed_control *ctrl = reinterpret_cast<subclassed_control<_Chr, _Base> *>(GetPropW(hwnd, reinterpret_cast<LPCWSTR>(getpropertyatom())));
        _ASSERT(ctrl->_hwnd == hwnd || msg == WM_DESTROY || msg == WM_NCDESTROY);
        if ( ctrl )
        { return ctrl->wndproc( msg, wparam, lparam); }
        else
        { return win::defwndproc<_Chr>( hwnd, msg, wparam, lparam ); }
      }

      subclassed_control() { }
    public:

      HWND gethwnd()
      { return this->_hwnd; }

      void reflectnotify(HWND hwnd)
      { _registeredfornotify.insert(hwnd); }

      void reflectcommand(HWND hwnd)
      { _registeredforcommand.insert(hwnd); }

      const RECT &getmargins()
      {
        static RECT r = { 2, 2, 2, 2 };
        return r;
      }

      subclassed_control( DWORD style, DWORD exstyle, const _Chr *windowclass, const std::basic_string<_Chr> &windowtext, bool enabled = true )
        : _windowclass(windowclass), _windowtext(windowtext), _style(style | WS_CHILD), _exstyle(exstyle), _enabled( enabled )
      { }

      virtual HWND createwindow(abstractwindow &owner)
      {
        HWND hwnd = NULL;
        if (!(hwnd = win::createwindow(_exstyle, _windowclass, _windowtext, _style & ~WS_VISIBLE, 0, 0, 0, 0, owner.gethwnd(), 0, 0, 0)))
        { throw windowsexception(GetLastError()); }
        return hwnd;
      }

      virtual void init(abstractwindow &wnd)
      { 
        if (this->_hwnd)
        { return; }
        control<_Base, group>::init(wnd);
        this->_hwnd = createwindow(wnd);
        SetPropW(this->_hwnd, reinterpret_cast<LPCWSTR>(getpropertyatom()), reinterpret_cast<HANDLE>(this));
        _oldWndProc = reinterpret_cast<WNDPROC>(win::setwindowlongptr<_Chr>(this->_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(staticWndProc)));
        win::sendmessage<_Chr>(this->_hwnd, WM_SETFONT, (WPARAM)this->getparentdialog().getfont().get(), FALSE);
      }

      virtual void addtowindow(abstractwindow &wnd, int x, int y, unsigned int w, unsigned int h)
      {
        _wnd = &wnd;
        MoveWindow(this->_hwnd, x, y, w, h, FALSE);
        ShowWindow(this->_hwnd, this->getvisible() ? SW_SHOWNORMAL : SW_HIDE);
        EnableWindow(this->_hwnd, this->getenabled());
      }
  };
}