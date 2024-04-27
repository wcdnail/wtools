#pragma once

namespace quickdialog
{
  enum combo_styles
  {
      cbDropdown = CBS_DROPDOWN,
      cbDropdownlist = CBS_DROPDOWNLIST,
      cbDefaultDropdown = CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_TABSTOP,
  };

  template<typename _Chr>
  class basic_combo_box : public subclassed_control<_Chr, basic_combo_box<_Chr> >
  {
    protected:
      std::vector<std::basic_string<_Chr> > _items;
      std::basic_string<_Chr> &_selectedvalue;
      int &_selectedindex;
      int _dummyselectedindex;
      WTL::CComboBox* _combo;

    public:

      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd)
          return;

        subclassed_control<_Chr, basic_combo_box<_Chr> >::init(wnd);
        wnd.reflectcommand(this->_hwnd);

        for(typename std::vector<std::basic_string<_Chr> >::iterator it = _items.begin(); it != _items.end(); ++it)
            win::sendmessage<_Chr>(this->_hwnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(it->c_str()));

        if (_selectedindex >= 0)
            win::sendmessage<_Chr>(this->_hwnd, CB_SETCURSEL, _selectedindex, 0);
        else
            win::sendmessage<_Chr>(this->_hwnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(_selectedvalue.c_str()));

        if (NULL != _combo)
          _combo->Attach(this->_hwnd);
      }

      template<typename _It>
      basic_combo_box(std::basic_string<_Chr> &selectedvalue, _It firstitem, _It listitem, unsigned style = CBS_AUTOHSCROLL | CBS_DROPDOWN | WS_TABSTOP)
        : subclassed_control<_Chr, basic_combo_box<_Chr> >(style, WS_EX_CLIENTEDGE, LITERAL(_Chr, "COMBOBOX"), selectedvalue),
          _dummyselectedindex(-1),
          _selectedindex(_dummyselectedindex),
          _selectedvalue(selectedvalue),
          _combo(NULL)
      {
        _items.assign(firstitem, listitem);
      }

      template<typename _It>
      basic_combo_box(WTL::CComboBox& combo, std::basic_string<_Chr> &selectedvalue, _It firstitem, _It listitem, unsigned style = CBS_AUTOHSCROLL | CBS_DROPDOWN | WS_TABSTOP)
          : subclassed_control<_Chr, basic_combo_box<_Chr> >(style, WS_EX_CLIENTEDGE, LITERAL(_Chr, "COMBOBOX"), selectedvalue),
          _dummyselectedindex(-1),
          _selectedindex(_dummyselectedindex),
          _selectedvalue(selectedvalue),
          _combo(&combo)
      {
          _items.assign(firstitem, listitem);
      }

      template<typename _Iter>
      basic_combo_box(int &selectedindex, _Iter firstitem, _Iter listitem, unsigned style = cbDefaultDropdown)
        : subclassed_control<_Chr, basic_combo_box<_Chr> >(style, WS_EX_CLIENTEDGE, LITERAL(_Chr, "COMBOBOX"),  LITERAL(_Chr, "")),
          _dummyselectedindex(-1),
          _selectedindex(selectedindex),
          _selectedvalue(subclassed_control<_Chr, basic_combo_box<_Chr> >::_windowtext),
          _combo(NULL)
      {
        _items.assign(firstitem, listitem);
      }

      template<typename _Iter>
      basic_combo_box(WTL::CComboBox& combo, int &selectedindex, _Iter firstitem, _Iter listitem, unsigned style = cbDefaultDropdown)
          : subclassed_control<_Chr, basic_combo_box<_Chr> >(style, WS_EX_CLIENTEDGE, LITERAL(_Chr, "COMBOBOX"),  LITERAL(_Chr, "")),
          _dummyselectedindex(-1),
          _selectedindex(selectedindex),
          _selectedvalue(subclassed_control<_Chr, basic_combo_box<_Chr> >::_windowtext),
          _combo(&combo)
      {
          _items.assign(firstitem, listitem);
      }

      SIZE sizeofitems()
      {
        SIZE result = {};
        for(typename std::vector<std::basic_string<_Chr> >::iterator it = _items.begin(); it != _items.end(); it++)
        {
          SIZE s = sizeofstr(*it);
          result.cx = max(result.cx, s.cx);
          result.cy = max(result.cy, s.cy);
        }
        return result;
      }

      SIZE requestsize()
      {
        // HACK: This is known to be a pixel out when visual styles are used
        // However this is preferable to a large amount of theming code and a dependency on uxtheme.lib, 
        // which breaks compatibility with older operating systems. I could dynamically link to uxtheme.lib,
        // and do it properly but thats a lot of work and it's only one pixel...
        SIZE result;
        RECT clientrect;
        COMBOBOXINFO cbi = {};
        cbi.cbSize = sizeof(cbi);
        SIZE itemsize = sizeofitems();
        GetComboBoxInfo(this->_hwnd, &cbi);
        result.cy = cbi.rcItem.bottom + cbi.rcItem.top; // Margin gets added on to the bottom...
        GetClientRect(this->_hwnd, &clientrect);
        result.cx = (clientrect.right - clientrect.left) - (cbi.rcItem.right - cbi.rcItem.left);
        result.cx += itemsize.cx;
        result.cx = max(result.cx, 50);
        return result;
      }

      virtual void ondestroy()
      {
        _selectedindex = win::sendmessage<_Chr>(this->_hwnd, CB_GETCURSEL, 0, 0) & 0xFFFFFFFF;
        win::retrievewindowtext(this->_hwnd, _selectedvalue);
        subclassed_control<_Chr, basic_combo_box<_Chr> >::ondestroy();
      }

      void addtowindow(abstractwindow &wnd, int x, int y, unsigned int w, unsigned int h)
      {
        int itemheight = win::sendmessage<_Chr>(this->_hwnd, CB_GETITEMHEIGHT, 0, 0) & 0xFFFFFFFF;
        h += itemheight * (_items.size() < 12 ? (_items.size() & 0xFFFFFFFF) : 8);
        subclassed_control<_Chr, basic_combo_box<_Chr> >::addtowindow(wnd, x, y, w, h);
      }
  };

  CTRL_TYPEDEFS(combo_box)
}