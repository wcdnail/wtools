#pragma once

namespace quickdialog
{
  template<typename _Chr, typename _Layout>
  class basic_groupbox : public container_control<_Chr, basic_groupbox<_Chr, _Layout>, _Layout>
  {
    public:
      const RECT &getpadding()
      { 
        static RECT padding = { 2, 2, 2, 2 };
        padding.top = this->sizeofwindowtext().cy;
        return padding;
      }

      virtual void init(abstractwindow &wnd)
      {
        this->_ownerwindow = &wnd;
        container_control<_Chr, basic_groupbox<_Chr, _Layout>, _Layout>::init(wnd);
      }

    public:
      basic_groupbox(const std::basic_string<_Chr> &windowtext, _Layout layout = _Layout())
        : container_control<_Chr, basic_groupbox<_Chr, _Layout>, _Layout>(BS_GROUPBOX | WS_TABSTOP, 0, LITERAL(_Chr, "BUTTON"), windowtext)
      { }
  };

  CONTAINER_CTRL_TYPEDEFS(groupbox)
}