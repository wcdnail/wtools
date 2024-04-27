#pragma once

namespace quickdialog
{
  template<typename _Chr, class _Base>
  class listbox_base : public subclassed_control<_Chr, _Base>
  {
    protected:
      typedef subclassed_control<_Chr, _Base> super;
      std::vector<std::basic_string<_Chr> > _items;
      std::tr1::function<void (HWND, int)> _onsetcursel;

      SIZE sizeofitems()
      {
        SIZE result = {};
        for(typename std::vector<std::basic_string<_Chr> >::iterator it = _items.begin(); it != _items.end(); it++)
        {
          SIZE s = subclassed_control<_Chr, _Base>::sizeofstr(*it);
          result.cx = max(result.cx, s.cx);
          result.cy = max(result.cy, s.cy);
        }
        return result;
      }

    public:
      template<typename _Iter>
      listbox_base( DWORD style, DWORD exstyle, _Iter firstitem, _Iter lastitem)
        : super(style, exstyle, LITERAL(_Chr, "LISTBOX"), LITERAL(_Chr, "")),
        _onsetcursel()
      { 
        _items.assign(firstitem, lastitem);
      }

      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd)
            return;

        super::init(wnd);

        for(typename std::vector<std::basic_string<_Chr> >::iterator it = _items.begin(); it != _items.end(); ++it)
          win::sendmessage<_Chr>(this->_hwnd, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(it->c_str()));
      }

      SIZE requestsize()
      {
        SIZE result = sizeofitems();
        RECT clientrect = { 0, 0, result.cx + 3, result.cy * (_items.size() < 12 ? _items.size() & 0xFFFFFFFF : 8) };
        AdjustWindowRectEx(&clientrect, super::_style, false, super::_exstyle);
        result.cx = clientrect.right - clientrect.left;
        result.cy = clientrect.bottom - clientrect.top;
        return result;
      }

    private:
      virtual void OnSetCurSel(int item)
      {
          if (_onsetcursel)
              _onsetcursel(gethwnd(), item);
      }

      LRESULT wndproc(UINT msg, WPARAM wparam, LPARAM lparam)
      {
        switch (msg)
        {
        case WM_LBUTTONDBLCLK:
          OnSetCurSel((int)(win::sendmessage<_Chr>(this->_hwnd, LB_GETCURSEL, 0, 0) & 0xFFFFFFFF));
          break;
        }
        return super::wndproc(msg, wparam, lparam);
      }
  };

  template<typename _Chr>
  class basic_listbox : public listbox_base<_Chr, basic_listbox<_Chr> >
  {
    protected:
      int &_selectedindex;

    public:
      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd)
        { return; }
        listbox_base<_Chr, basic_listbox<_Chr> >::init(wnd);
        if (_selectedindex >= 0)
        { win::sendmessage<_Chr>(this->_hwnd, LB_SETCURSEL, _selectedindex, 0); }
      }

      template<typename _Iter>
      basic_listbox(int &selectedindex, _Iter firstitem, _Iter lastitem)
        : listbox_base<_Chr, basic_listbox<_Chr> >(LBS_NOINTEGRALHEIGHT, WS_EX_CLIENTEDGE, firstitem, lastitem),
          _selectedindex(selectedindex)
      { }

      template<typename _Iter, class _OnSetCurSel>
      basic_listbox(int &selectedindex, _Iter firstitem, _Iter lastitem, _OnSetCurSel onsetcursel)
          : listbox_base<_Chr, basic_listbox<_Chr> >(LBS_NOTIFY | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT, WS_EX_CLIENTEDGE, firstitem, lastitem),
          _selectedindex(selectedindex)
      {
        _onsetcursel = onsetcursel;
      }

      virtual void ondestroy()
      {
        _selectedindex = win::sendmessage<_Chr>(this->_hwnd, LB_GETCURSEL, 0, 0) & 0xFFFFFFFF;
        listbox_base<_Chr, basic_listbox<_Chr> >::ondestroy();
      }
  };

  template<typename _Chr>
  class basic_multiselectlistbox : public listbox_base<_Chr, basic_multiselectlistbox<_Chr> >
  {
    protected:
      std::vector<int> &_selectedindices;

    public:
      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd)
        { return; }
        listbox_base<_Chr, basic_multiselectlistbox<_Chr> >::init(wnd);
        for(typename std::vector<int>::iterator it = _selectedindices.begin(); it != _selectedindices.end(); ++it)
        { win::sendmessage<_Chr>(this->_hwnd, LB_SETSEL, TRUE, *it); }
      }

      template<typename _Iter>
      basic_multiselectlistbox(std::vector<int> &selectedindices, bool useextendedsel, _Iter firstitem, _Iter lastitem)
        : listbox_base<_Chr, basic_multiselectlistbox<_Chr> >(LBS_MULTIPLESEL | (useextendedsel ? LBS_EXTENDEDSEL : 0), WS_EX_CLIENTEDGE, firstitem, lastitem),
          _selectedindices(selectedindices)
      { }

      virtual void ondestroy()
      {
        int count = win::sendmessage<_Chr>(this->_hwnd, LB_GETCOUNT, 0, 0) & 0xFFFFFFFF;
        _selectedindices.clear();
        for(int i = 0; i < count; ++i)
        { 
          if (win::sendmessage<_Chr>(this->_hwnd, LB_GETSEL, i, 0))
          { _selectedindices.push_back(i); }
        }
        listbox_base<_Chr, basic_multiselectlistbox<_Chr> >::ondestroy();
      }      
  };

  CTRL_TYPEDEFS(listbox)
  CTRL_TYPEDEFS(multiselectlistbox)
}