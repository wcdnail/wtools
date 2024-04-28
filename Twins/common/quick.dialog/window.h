#pragma once

namespace quickdialog
{
  enum Buttons { btnNone = 0, btnClose = 1, btnMin = 2, btnMax = 4, btnHelp = 8 };

  template<typename _Chr, class _Layout>
  class window : public abstractdialogwindow
  {
  private:
    bool _closed;
    shared_handle<HFONT> _font;
    HWND _hwnd;
    ctrlvector _controls;
    _Layout _layout;
    std::tr1::unordered_set<HWND> _registeredfornotify, _registeredforcommand;
    bool _inited;
    void reflectnotify(HWND hwnd)
    { _registeredfornotify.insert(hwnd); }

    void reflectcommand(HWND hwnd)
    { _registeredforcommand.insert(hwnd); }

    LRESULT CALLBACK WndProc(UINT msg, WPARAM wparam, LPARAM lparam)
    {
      switch (msg)
      {
        case WM_NOTIFY:
          {
            if (_registeredfornotify.find(reinterpret_cast<NMHDR *>(lparam)->hwndFrom) != _registeredfornotify.end())
            { return win::sendmessage<_Chr>(reinterpret_cast<NMHDR *>(lparam)->hwndFrom, msg, wparam, lparam); }
            break;
          }
        case WM_COMMAND:
          {
            if (!lparam)
            {
              if (LOWORD(wparam) == IDOK)
              { lparam = reinterpret_cast<LPARAM>(this->_defaultbutton); }
              else if (LOWORD(wparam) == IDCANCEL)
              { lparam = reinterpret_cast<LPARAM>(this->_cancelbutton); }
            }
            
            if (_registeredforcommand.find(reinterpret_cast<HWND>(lparam)) != _registeredforcommand.end())
            { return win::sendmessage<_Chr>(reinterpret_cast<HWND>(lparam), msg, wparam, lparam); }
            break;
          }
        case WM_CLOSE:
          _closed = true; //Fall through intended
        default:
          return win::defdlgproc<_Chr>(_hwnd, msg, wparam, lparam);
      }
      return 0;
    }

    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
      if (msg == WM_CREATE)
      { SetWindowLongPtr(hwnd, GWLP_USERDATA, *(LONG *)lparam); }
      window *ptr = reinterpret_cast<window *>(win::getwindowlongptr<_Chr>(hwnd, GWLP_USERDATA));
      if ( ptr )
      { return ptr->WndProc(msg, wparam, lparam); }
      return win::defdlgproc<_Chr>(hwnd, msg, wparam, lparam);
    }
  
    static void regwindowclass()
    {
      typename winstructs<_Chr>::WndClassEx wndclass;
      wndclass.cbSize = sizeof(wndclass);
      wndclass.cbWndExtra = DLGWINDOWEXTRA;
      wndclass.cbClsExtra = 0;
      wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
      wndclass.hInstance = GetModuleHandle(NULL);
      wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
      wndclass.hIcon = NULL;
      wndclass.hIconSm = NULL;
      wndclass.lpszClassName = string_literal<_Chr>::stringliteral("rfdialogwindowA", L"rfdialogwindowW");
      wndclass.lpszMenuName = NULL;
      wndclass.style = 0;
      wndclass.lpfnWndProc = StaticWndProc;
      win::registerclassex(wndclass); // Ignore return code - we call this every time to ensure the class is registered.
    }

    int ModalLoop(HWND owner)
    {
      _closed = false;
      MSG msg;
      _returnvalue = qdCancel;
        
      if (!_hwnd)
      { return 0; /*TODO: throw error here */ }

      if (owner == NULL)
      { owner = GetActiveWindow(); }
      if (owner == _hwnd)
      { owner = NULL; }

      HWND capturewnd = GetCapture();
      if (capturewnd)
      { win::sendmessage<_Chr>(capturewnd, WM_CANCELMODE, 0, 0); }

      if (owner)
      { EnableWindow( owner, FALSE ); }

      while (!_closed && win::getmessage<_Chr>( &msg, 0L, 0L, 0L ))
      { 
        if (msg.message == WM_QUIT)
        {
          _closed = true;
          win::postmessage<_Chr>(NULL, WM_QUIT, 0, 0);
          break;
        }

        if (!win::isdialogmessage<_Chr>(_hwnd, &msg))
        { 
          TranslateMessage(&msg);
          win::dispatchmessage<_Chr>(&msg);
        }
      }
      NotifyWinEvent(EVENT_SYSTEM_DIALOGEND, _hwnd, 0, 0);
        
      if (IsWindow(_hwnd))
      { DestroyWindow(_hwnd); }
      if (owner)
      { EnableWindow(owner, TRUE); }
      return _returnvalue;
    }

    unsigned int GetStyle(int buttons, bool resizeable)
    {
      unsigned int style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED | WS_POPUP;
      style |= (buttons & btnMax) ? WS_MAXIMIZEBOX : 0;
      style |= (buttons & btnMin) ? WS_MINIMIZEBOX : 0;
      return style | (resizeable ? WS_THICKFRAME : DS_MODALFRAME | DS_NOIDLEMSG | DS_ABSALIGN);
    }

    unsigned int GetExStyle(int buttons, bool resizeable)
    {
      unsigned int exstyle = WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT | (resizeable ? 0 : WS_EX_DLGMODALFRAME);
      return exstyle | ((buttons & btnHelp) ? WS_EX_CONTEXTHELP : 0);
    }

  public:
    shared_handle<HFONT> getfont()
    { return _font; }

    HWND gethwnd()
    { return _hwnd; }

    shared_handle<HDC> getDC()
    { return shared_handle<HDC>( GetDC(_hwnd), release_dc(_hwnd)); }
    
    const RECT &getpadding()
    {
      static RECT padding = { 8, 8, 8, 8 };
      return padding;
    };
    
    void reworklayout(int minwidth)
    {
      RECT rect = {}, clientrect = {};
      GetWindowRect(_hwnd, &rect);
      GetClientRect(_hwnd, &clientrect);
      minwidth -= (rect.right - rect.left) - (clientrect.right - clientrect.left);
      init();
      SIZE sz = _layout(*this, _controls, getpadding(), minwidth);
      reposition(rect.left, rect.top, sz.cx, sz.cy);
    }

    window(const std::basic_string<_Chr> &title, HWND parent, int buttons, bool resizeable, ctrlvector &controls, _Layout layout)
      : _hwnd(NULL),  _font(NULL), _layout(layout), _controls(controls), _inited(false)
    {  
      regwindowclass();
      _hwnd = win::createwindow(GetExStyle(buttons, resizeable), 
                                string_literal<_Chr>::stringliteral("rfdialogwindowA", L"rfdialogwindowW"), 
                                title.c_str(), 
                                GetStyle(buttons, resizeable), 
                                0, 0, 
                                300, 100, 
                                parent, 
                                NULL, 
                                NULL, 
                                this);
      if (!_hwnd)
      { throw lasterrorexception(); }
      typename winstructs<_Chr>::non_client_metrics ncm = {};
      getnonclientmetrics(ncm);
      _font = shared_handle<HFONT>(win::createfontindirect(ncm.lfMessageFont), delete_object());
      win::sendmessage<_Chr>(_hwnd, WM_SETFONT, (WPARAM)_font.get(), FALSE);
      win::sendmessage<_Chr>(_hwnd, WM_SETICON, ICON_SMALL, 0);
      win::sendmessage<_Chr>(_hwnd, WM_SETICON, ICON_BIG, 0);
      HMENU menu = GetSystemMenu(_hwnd, FALSE);
      if (!resizeable)
      {
        RemoveMenu(menu, SC_MAXIMIZE, 0);
        RemoveMenu(menu, SC_MINIMIZE, 0);
        RemoveMenu(menu, SC_RESTORE, 0);
        RemoveMenu(menu, SC_SIZE, 0);
      }
    }

    void getnonclientmetrics(typename winstructs<_Chr>::non_client_metrics &ncm)
    {
      ncm.cbSize = sizeof(ncm);

    #if WINVER >= 0x600 && !defined(__GNUC__)
      if (LOBYTE(LOWORD(GetVersion())) < 6)
      { ncm.cbSize -= sizeof(ncm.iPaddedBorderWidth); }
    #endif

      if (win::systemparametersinfo<_Chr>(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0) == 0)
      { throw windowsexception(GetLastError()); }
    };

    //There is no way I've found to calc this automatically, have to do it the hard (read: flaky) way
    int minwindowwidth() // The width of the window if the window caption is read out in full...
    {
      int buttoncount = 0;
      
      typename winstructs<_Chr>::non_client_metrics ncm = {};
      getnonclientmetrics(ncm);
      LONG style = win::getwindowlong<_Chr>(_hwnd, GWL_STYLE);
      if (style & WS_SYSMENU)
      { 
        buttoncount = (win::sendmessage<_Chr>(_hwnd, WM_GETICON, ICON_BIG, 0) && 
                       win::sendmessage<_Chr>(_hwnd, WM_GETICON, ICON_SMALL, 0)) ? 2 : 1;
        if (style & (WS_MAXIMIZEBOX | WS_MINIMIZEBOX))
        { buttoncount += 2; }
      }
      unique_handle<HFONT, delete_object> f(win::createfontindirect(ncm.lfCaptionFont), delete_object());
      unique_handle<HDC, delete_object> dc(GetDC(_hwnd), delete_object());
      
      HGDIOBJ oldfont = SelectObject(dc.get(), f.get());
      std::basic_string<_Chr> caption;
      win::retrievewindowtext(_hwnd, caption);
      
      RECT r = {}, w;
      GetWindowRect(_hwnd, &w);
      if (!win::drawtext(dc.get(), caption, r, DT_CALCRECT | DT_SINGLELINE))
      { throw windowsexception(GetLastError()); }
      SelectObject(dc.get(), oldfont);
      r.right += (ncm.iCaptionWidth) * buttoncount;
      AdjustWindowRect(&r, style, false);
      return (r.right - r.left) + 13; // magic number - found by experimentation. Anyone with a better way please let me know.
    }

    // TODO: make ATL/WTL compatible
    int showdialog(HWND owner = NULL)
    {
      RECT ownerrect, rect;
      if (owner)
      { 
        SetParent(_hwnd, owner); 
        GetWindowRect(owner, &ownerrect);
      }
      else // Use primary monitor
      { 
        ownerrect.left = ownerrect.top = 0;
        ownerrect.right = GetSystemMetrics(SM_CXSCREEN);
        ownerrect.bottom = GetSystemMetrics(SM_CYSCREEN);
      }
      GetWindowRect(_hwnd, & rect);
      rect.right -= rect.left;
      rect.bottom -= rect.top;
      rect.left = ownerrect.left + ((ownerrect.right - ownerrect.left) / 2) - (rect.right / 2); // NB: Not (ownerrect.right + ownerrect.left) / 2 because that could potentially overflow.
      rect.top = ownerrect.top + ((ownerrect.bottom - ownerrect.top) / 2) - (rect.bottom / 2); // As above.
      SetWindowPos(_hwnd, NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW | SWP_NOSIZE);
      _closed = false;
      return ModalLoop(owner);
    }

    void init()
    {
      if (_inited)
      { return; }
      for(unsigned int i = 0; i < _controls.size(); ++i)
      { _controls[i]->init(*this); }

      HMENU menu = GetSystemMenu(_hwnd, FALSE);
      if (menu && !_cancelbutton)
      { 
        int count = GetMenuItemCount(menu);
        RemoveMenu(menu, count - 2, MF_BYPOSITION);
        RemoveMenu(menu, SC_CLOSE, 0);
      }

      _inited = true;
    }

    void reposition(int x, int y, int w, int h)
    { 
      RECT wndrect, clientrect;
      GetWindowRect(_hwnd, &wndrect);
      GetClientRect(_hwnd, &clientrect);
      if (!MoveWindow(_hwnd, x, y, w + (wndrect.right - wndrect.left) - (clientrect.right - clientrect.left), h + (wndrect.bottom - wndrect.top) - (clientrect.bottom - clientrect.top), true))
      { throw lasterrorexception(); }
    }
  };
}