#pragma once

namespace quickdialog
{
  class dialogbase
  {
    public:
      int _buttons;
      bool _sizeable;
    public:
      dialogbase(bool sizeable = false, bool helpbutton = false)
        : _buttons(btnClose | (sizeable ? (btnMax | btnMin) : 0) | (helpbutton ? btnHelp : 0)), 
          _sizeable(sizeable)
      { }
  };

  template<typename _Chr, class _Layout>
  class basic_dialog : public dialogbase,
                       public container_operators<basic_dialog<_Chr, _Layout> >
  {
    private:
      std::basic_string<_Chr> _title;
      window<_Chr, _Layout>* _ui;

    public:
      basic_dialog(std::basic_string<_Chr> title, bool sizeable = false, bool helpbutton = false)
        : dialogbase(sizeable, helpbutton), _title( title ), _ui(NULL)
      { }

    dialogresult show(_Layout layout = _Layout())
    {
      return show(NULL, -1, layout);
    }

    dialogresult show(HWND parent, int cx, _Layout layout = _Layout())
    {
        window<_Chr, _Layout> d(_title, parent, _buttons, _sizeable, container_operators<basic_dialog<_Chr, _Layout> >::_controls, layout);
        d.reworklayout(-1 == cx ? d.minwindowwidth() : cx);
        _ui = &d;
        dialogresult rv = static_cast<dialogresult>(d.showdialog());
        _ui = NULL;
        return rv;
    }

    void close(int rv)
    {
        if (NULL != _ui)
        {
            _ui->_returnvalue = rv;
            win::sendmessage<_Chr>(_ui->gethwnd(), WM_CLOSE, 0, 0);
        }
    }
  };

  CONTAINER_CTRL_TYPEDEFS(dialog)
}
