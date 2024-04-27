#pragma once

namespace quickdialog
{
  template<typename _Chr, typename _Layout = layout_engine>
  class basic_tab : public container_control<_Chr, basic_tab<_Chr, _Layout>, _Layout>
  {
    private:
      static const _Chr *TAB_CLASS;
      static void registertab()
      {
        typename winstructs<_Chr>::WndClassEx wndclass = {};
        wndclass.cbSize = sizeof(wndclass);
        wndclass.hInstance = GetModuleHandle(NULL);
        wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
        wndclass.lpszClassName = TAB_CLASS;
        wndclass.lpfnWndProc = StaticWndProc;
        if (!win::registerclassex(wndclass))
        { 
          DWORD err = GetLastError(); 
          if (err != ERROR_CLASS_ALREADY_EXISTS)
          { throw windowsexception(err); }
        }
      }
      
      static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
      { 
        return win::defwndproc<_Chr>(hwnd, msg, wparam, lparam); 
      }
    
    protected:
      /* In order to get the tab control theming correctly, I need to create a dialog with a dialog procedure and 
         call EnableThemeDialogTexture (dynamically loaded for backwards compatibility). Ye gods, could they not have made this easier?*/
      virtual HWND createwindow(abstractwindow &owner)
      {
        HWND hwnd = NULL;
        struct
        { 
          DLGTEMPLATE dlg;
          WORD menu;
          WORD classname;
          wchar_t windowtext[2048+1];
        } memdlg = {};
        
        wcsncpy_s(memdlg.windowtext, string::convert<_Chr, wchar_t>(this->_windowtext).c_str(), (sizeof(memdlg.windowtext) / sizeof(wchar_t)) - 1);
        memdlg.dlg.style = this->_style & ~WS_VISIBLE;
        memdlg.dlg.dwExtendedStyle = this->_exstyle;
        if (!(hwnd = CreateDialogIndirectW(GetModuleHandle(NULL), &memdlg.dlg, owner.gethwnd(), dialoginit)))
        { throw windowsexception(GetLastError()); }
        return hwnd;
      }

      static INT_PTR _stdcall dialoginit(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
      {
        if (msg == WM_INITDIALOG)
        { win::enablethemedialogtexture(hwnd, 6); }
        return false;
      }
    public:
      basic_tab(std::basic_string<_Chr> text)
        : container_control<_Chr, basic_tab<_Chr, _Layout>, _Layout>(0, WS_EX_TRANSPARENT, reinterpret_cast<const _Chr *>(WC_DIALOG), text)
      { }

      void init(abstractwindow &wnd)
      {
        container_control<_Chr, basic_tab<_Chr, _Layout>, _Layout>::init(wnd);        
      }

      virtual void addtowindow(abstractwindow &wnd, int x, int y, unsigned int w, unsigned int h)
      {
        this->_wnd = &wnd;
        MoveWindow(this->_hwnd, x, y, w, h, FALSE);
        // Tabs remain invisible until the main tab control is shown.
      }

      virtual LRESULT wndproc(UINT msg, WPARAM wparam, LPARAM lparam)
      {
        switch(msg)
        {
          case WM_ERASEBKGND:
            ;//return 1;
        }
        return container_control<_Chr,  basic_tab<_Chr, _Layout>, _Layout>::wndproc(msg, wparam, lparam);
      }
  };

  template<typename _Chr, typename _Layout>
  const _Chr *basic_tab<_Chr, _Layout>::TAB_CLASS = string_literal<_Chr>::stringliteral("rftabwindowA", L"rftabwindowW");

  enum tab_orientation 
  { 
    top = 0, 
    left = TCS_VERTICAL | TCS_MULTILINE, 
    bottom = TCS_BOTTOM, 
    right = TCS_RIGHT | TCS_VERTICAL | TCS_MULTILINE
  };

  enum tab_style 
  { 
    normal = 0, 
    hottrack = TCS_HOTTRACK,
    multiline = TCS_MULTILINE,
    scrollopposite = TCS_SCROLLOPPOSITE,
    rightjustify = TCS_RIGHTJUSTIFY,
    focusnever = TCS_FOCUSNEVER,
    focusonmousedown = TCS_FOCUSONBUTTONDOWN
  };

  enum tab_display
  {
    tabs = TCS_TABS,
    buttons = TCS_BUTTONS, 
    flatbuttons = TCS_FLATBUTTONS | TCS_BUTTONS,
    multiselectbuttons = TCS_BUTTONS | TCS_MULTISELECT, 
    multiselectflatbuttons = TCS_FLATBUTTONS | TCS_BUTTONS | TCS_MULTISELECT
  };

  template<typename _Chr>
  class basic_tab_control : public subclassed_control<_Chr, basic_tab_control<_Chr> >
  {
    private:
      typedef std::tr1::shared_ptr<basic_tab<_Chr> > tab_ptr;
      typedef std::vector<tab_ptr> tab_vector;
      tab_vector _tabs;
      unsigned int _initialtab;
    public:
      basic_tab_control(unsigned int initialtab = 0, tab_orientation orientation = top, tab_style style = normal, tab_display display = tabs)
        : subclassed_control<_Chr, basic_tab_control<_Chr> >(orientation | style | display, 0, LITERAL(_Chr, "SysTabControl32"), LITERAL(_Chr, "")),
         _initialtab( initialtab )
      { }

      basic_tab_control<_Chr> operator+(basic_tab<_Chr> &ctrl)
      { _tabs.push_back(tab_ptr(new basic_tab<_Chr>(ctrl))); return *this; }

      virtual LRESULT wndproc(UINT msg, WPARAM wparam, LPARAM lparam)
      {
        LRESULT selection;
        switch (msg)
        {
          case WM_NOTIFY:
            NMHDR *header = reinterpret_cast<NMHDR *>(lparam);
            switch (header->code)
            {
              case TCN_SELCHANGING:
                selection = win::sendmessage<_Chr>(this->_hwnd, TCM_GETCURSEL, 0, 0);
                ShowWindow(_tabs[selection].get()->gethwnd(), SW_HIDE);
                break;
              case TCN_SELCHANGE:
                selection = win::sendmessage<_Chr>(this->_hwnd, TCM_GETCURSEL, 0, 0);
                ShowWindow(_tabs[selection].get()->gethwnd(), SW_SHOW);
                break;
            }
            break;
        }
        return subclassed_control<_Chr, basic_tab_control<_Chr> >::wndproc(msg, wparam, lparam);
      }

      virtual void init(abstractwindow &wnd)
      { 
        if (this->_hwnd)
        { return; }
        subclassed_control<_Chr, basic_tab_control<_Chr> >::init(wnd);
        wnd.reflectnotify(this->_hwnd);
        for(unsigned int i = 0; i < _tabs.size(); ++i)
        {
          _tabs[i].get()->init(wnd);
          typename winstructs<_Chr>::tc_item tcitem = {};
          tcitem.cchTextMax = win::sendmessage<_Chr>(_tabs[i].get()->gethwnd(), WM_GETTEXTLENGTH, 0, 0) & 0xFFFFFFFF;
          _Chr *buffer = new _Chr[tcitem.cchTextMax + 1];
          win::sendmessage<_Chr>(_tabs[i].get()->gethwnd(), WM_GETTEXT, tcitem.cchTextMax + 1, reinterpret_cast<LPARAM>(buffer));
          tcitem.pszText = buffer;
          tcitem.mask = TCIF_TEXT;
          win::sendmessage<_Chr>(this->_hwnd, PICKMESSAGE(_Chr, TCM_INSERTITEM), i, reinterpret_cast<LPARAM>(&tcitem));
          delete[] buffer;
        }
      }

      SIZE requestsize()
      {
        RECT r = {};
        for(typename tab_vector::iterator it = _tabs.begin(); it != _tabs.end(); ++it)
        {
          SIZE s = it->get()->requestsize();
          r.right = max((LONG) s.cx, r.right);
          r.bottom = max((LONG) s.cy, r.bottom);
        }
        win::sendmessage<_Chr>(this->_hwnd, TCM_ADJUSTRECT, TRUE, reinterpret_cast<LPARAM>(&r));
        SIZE s = { r.right - r.left, r.bottom - r.top };
        return s;
      }

      void addtowindow(abstractwindow &wnd, int x, int y, unsigned int w, unsigned int h)
      {
        subclassed_control<_Chr, basic_tab_control<_Chr> >::addtowindow(wnd, x, y, w, h);
        RECT r = { x, y, x + w, y + h };
        win::sendmessage<_Chr>(this->_hwnd, TCM_ADJUSTRECT, FALSE, reinterpret_cast<LPARAM>(&r));
        for(typename tab_vector::iterator it = _tabs.begin(); it != _tabs.end(); ++it)
        { it->get()->addtowindow(wnd, r.left, r.top, r.right - r.left, r.bottom - r.top); }
        if (_initialtab >= _tabs.size())
        { _initialtab = 0; }
        win::sendmessage<_Chr>(this->_hwnd, TCM_SETCURSEL, _initialtab, 0);
        ShowWindow(_tabs[ _initialtab ].get()->gethwnd(), SW_NORMAL);
      }
  };

  CTRL_TYPEDEFS(tab_control)
  CTRL_TYPEDEFS(tab)
}