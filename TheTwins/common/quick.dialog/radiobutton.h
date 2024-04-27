#pragma once

namespace quickdialog
{
  template<typename _Chr>
  class basic_radio_button : public button_base<_Chr, basic_radio_button<_Chr> >
  {
    private:
      std::tr1::function<void(abstractcontrol *sender, bool checked)> _clickfunctor;
      bool &_checkstate;
    public:
      basic_radio_button(std::basic_string<_Chr> text, bool &checked, bool startsgroup = false)
        : button_base<_Chr, basic_radio_button<_Chr> >(BS_AUTORADIOBUTTON | WS_TABSTOP | (startsgroup ? WS_GROUP : 0), 0, text, checked ? csChecked : csUnchecked),
          _checkstate(checked)
      { }

      template<class _ClickHandler>
      basic_radio_button(std::basic_string<_Chr> text, bool &checkstate, _ClickHandler clickhandler, bool startsgroup = false)
        : button_base<_Chr, basic_radio_button<_Chr> >(BS_AUTORADIOBUTTON | WS_TABSTOP | (startsgroup ? WS_GROUP : 0), 0, text, checkstate ? csChecked : csUnchecked),
          _checkstate(checkstate),
          _clickfunctor(clickhandler)
      { }

      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd) { return; }
        button_base<_Chr, basic_radio_button<_Chr> >::init(wnd);
        this->putstate(_checkstate ? csChecked : csUnchecked);
      }

      virtual void OnClick()
      { 
        if (_clickfunctor)
        { _clickfunctor(this, this->getstate() == csChecked); }
      }

      virtual SIZE requestsize()
      {
        SIZE s = this->sizeofwindowtext();
        s.cx += 24;
        s.cy += 5;
        return s;
      }

      virtual void ondestroy()
      {
        _checkstate = this->getstate() == csChecked;
        button_base<_Chr, basic_radio_button<_Chr> >::ondestroy();        
      }
  };

  template<typename _Chr, typename _Layout = layout_engine>
  class basic_radio_button_group : public control<basic_radio_button_group<_Chr, _Layout>, group>
  {
    private:
      unsigned int _nextindex;
      ctrlvector _controls;
      int &_selectedindex;
      abstractwindow *_ownerwindow;
      _Layout _layout;

      basic_radio_button_group(); //Hidden constructor

      class rb : public button_base<_Chr, rb>
      {
        std::basic_string<_Chr> _windowtext;
        basic_radio_button_group<_Chr, _Layout> &_grp;
        const unsigned int _index;

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

        SIZE requestsize()
        {
          SIZE s = this->sizeofwindowtext();
          s.cx += 24;
          s.cy += 5;
          return s;
        }

     public:
        rb(std::basic_string<_Chr> str, basic_radio_button_group<_Chr, _Layout> &grp)
          : button_base<_Chr, rb>(BS_AUTORADIOBUTTON | WS_TABSTOP | ((grp._nextindex == 0) ? WS_GROUP : 0), 0, str, (grp._nextindex == grp._selectedindex) ? csChecked : csUnchecked),
            _grp(grp), _index(grp._nextindex++)
        { }

        void setselectedindex()
        {
          if (this->getstate() == csChecked)
          { _grp._selectedindex = _index; }
        }

        virtual void init(abstractwindow &wnd)
        {
          button_base<_Chr, rb>::init(wnd);
          this->putstate((_grp._selectedindex == _index) ? csChecked : csUnchecked);
        }

        virtual void OnClick()
        { 
          setselectedindex();
        }

        virtual void ondestroy()
        {
          setselectedindex();
          button_base<_Chr, rb>::ondestroy();
        }
      };

      void addctrl(std::basic_string<_Chr> &str) 
      { _controls.push_back(ctrlptr(new rb(str, *this))); }
  public:
      basic_radio_button_group &operator<<(const _Chr *str) { _controls.push_back(ctrlptr(new rb(std::basic_string<_Chr>(str), *this))); return *this; }
      basic_radio_button_group &operator<<(std::basic_string<_Chr> &str) { _controls.push_back(ctrlptr(new rb(str, *this))); return *this; }
      basic_radio_button_group &operator<<(std::vector<std::basic_string<_Chr> > &list) { for_each(list.begin(), list.end(), addctrl); return *this; }

      basic_radio_button_group &operator<<(const sectionbreak &ctrl) { _controls.push_back(ctrlptr(new sectionbreak(ctrl))); return *this; }
      basic_radio_button_group &operator<<(const columnbreak &ctrl) {}

      basic_radio_button_group(int &selectedindex, _Layout layout = _Layout())
        : _selectedindex(selectedindex), _ownerwindow(NULL), _layout(layout), _nextindex(0)
      { }


      virtual const RECT &getpadding()
      { 
        static RECT padding = {};
        return padding;
      }

      virtual void init(abstractwindow &wnd)
      {
        if (_ownerwindow == NULL)
        { _ownerwindow = &wnd; }
        control<basic_radio_button_group<_Chr, _Layout>, group>::init(wnd);
        for(unsigned int i = 0; i < _controls.size(); ++i)
        { _controls[i]->init(wnd); }
      }

      SIZE requestsize()
      { 
        if (_ownerwindow)
        { return _layout(*_ownerwindow, _controls, getpadding(), 0); } 
        else
        { 
          SIZE s = {};
          return s;
        }
      }

    public:
      void addtowindow(abstractwindow &wnd, int x, int y, unsigned int w, unsigned int h)
      { 
        control<basic_radio_button_group<_Chr, _Layout>, group>::addtowindow(wnd, x, y, w, h);        
        RECT r = getpadding();
        if (_ownerwindow == &wnd)
        { 
          r.left += x;
          r.top +=y;
        }
        _layout(_ownerwindow == NULL ? wnd : *_ownerwindow, _controls, r, w);

        for (unsigned int i = 0; i < _controls.size(); ++i)
        { EnableWindow(_controls[i]->gethwnd(), _controls[i]->getenabled()); }
      }
  };

  CTRL_TYPEDEFS(radio_button)
  CTRL_TYPEDEFS(radio_button_group)
}