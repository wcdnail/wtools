//
// Author: Nikonov M. 2012.
// 

#pragma once

namespace quickdialog
{
  enum progressbar_styles
  {
      pbNone = 0,
  };

  template<typename _Chr, class _Base>
  class progressbar_base : public subclassed_control<_Chr, _Base>
  {
    public:
      typedef std::basic_string<_Chr> string_type;
      typedef subclassed_control<_Chr, _Base> super;

      progressbar_base(WTL::CProgressBarCtrl& bar, unsigned style, unsigned exstyle, unsigned cx, unsigned cy)
        : super(style, exstyle, LITERAL(_Chr, PROGRESS_CLASSA), string_type()),
          cx_(cx),
          cy_(cy),
          bar_(bar)
      {
      }

      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd)
            return;

        subclassed_control<_Chr, _Base>::init(wnd);
        bar_.Attach(this->_hwnd);
      }

      SIZE requestsize()
      {
        SIZE rv = { (int)cx_, (int)cy_ };
        return rv;
      }

    protected:
      unsigned cx_;
      unsigned cy_;
      WTL::CProgressBarCtrl& bar_;
    
      virtual void ondestroy()
      {
          subclassed_control<_Chr, _Base>::ondestroy();
      }
  };

  template<typename _Chr>
  class basic_progressbar : public progressbar_base<_Chr, basic_progressbar<_Chr> >
  {
    public:
      typedef progressbar_base<_Chr, basic_progressbar<_Chr> > super;

      basic_progressbar(WTL::CProgressBarCtrl& bar, unsigned cx = 120, unsigned cy = 24, unsigned style = pbNone)
        : super(bar, style, 0, cx, cy)
      {
      }
  };

  CTRL_TYPEDEFS(progressbar)
}
