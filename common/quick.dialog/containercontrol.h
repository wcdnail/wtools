#pragma once


namespace quickdialog
{

  template<typename _Chr, class _Base, class _Layout>
  class container_control : public subclassed_control<_Chr, _Base>,
                   public container_operators<_Base>
  {
    private:      
      container_control();
      _Layout _layout;
    protected:
      abstractwindow *_ownerwindow;
    public:
      container_control( DWORD style, DWORD exstyle, const _Chr *windowclass, const std::basic_string<_Chr> &windowtext )
        : subclassed_control<_Chr, _Base>( style, exstyle | WS_EX_CONTROLPARENT, windowclass, windowtext ),
          _ownerwindow( NULL )
      { }

      bool iscontainer() const
      { return true; }

      virtual const RECT &getpadding()
      { 
        static RECT padding = { 2, 2, 2, 2 };
        return padding;
      }

      virtual void init(abstractwindow &wnd)
      {
        if (_ownerwindow == NULL)
        { _ownerwindow = this; }
        subclassed_control<_Chr, _Base>::init(wnd);
        for(unsigned int i = 0; i < this->_controls.size(); ++i)
        { this->_controls[i]->init(*_ownerwindow); }
      }

      SIZE requestsize()
      { 
        if (_ownerwindow)
        { return _layout(*_ownerwindow, container_operators<_Base>::_controls, getpadding(), 0); } 
        else
        { 
          SIZE s = {};
          return s;
        }
      }

    public:
      void addtowindow(abstractwindow &wnd, int x, int y, unsigned int w, unsigned int h)
      { 
        subclassed_control<_Chr, _Base>::addtowindow(wnd, x, y, w, h);        
        RECT r = getpadding();
        if (_ownerwindow == &wnd)
        { 
          r.left += x;
          r.top +=y;
        }
        _layout(this->_ownerwindow == NULL ? wnd : *_ownerwindow, container_operators<_Base>::_controls, r, w);

        for (unsigned int i = 0; i < this->_controls.size(); ++i)
        { EnableWindow(this->_controls[i]->gethwnd(), this->_controls[i]->getenabled()); }
      }
  };
}