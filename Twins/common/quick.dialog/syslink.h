namespace quickdialog
{
  template<class _Chr>
  class basic_syslink : public subclassed_control<_Chr, basic_syslink<_Chr> >
  {
  private:
    std::tr1::function<bool (abstractcontrol *sender, const std::basic_string<_Chr> &link)> _clicklinkfunctor;
    virtual const RECT &getmargins()
    {
      static RECT margins = { 4, 2, 2, 4 };
      return margins;
    }

  public:

    basic_syslink(const std::basic_string<_Chr> &str) 
      : subclassed_control<_Chr, basic_syslink<_Chr> >(LWS_USEVISUALSTYLE, 0, LITERAL(_Chr, "SysLink"), str)
    { }

    basic_syslink(const _Chr *str)
      : subclassed_control<_Chr, basic_syslink<_Chr> >(LWS_USEVISUALSTYLE, LITERAL(_Chr, "SysLink"), str)
    { }

    virtual LRESULT wndproc(UINT msg, WPARAM wparam, LPARAM lparam)
    {
      PNMLINK plink;
      switch (msg)
      {
        case WM_NOTIFY:
          plink = reinterpret_cast<PNMLINK>(lparam);
          if (plink->hdr.hwndFrom == this->_hwnd) 
          {
            if ((plink->hdr.code == NM_CLICK) || (plink->hdr.code == NM_RETURN)) 
            {
              bool alreadyprocessed = false;
              if (_clicklinkfunctor)
              {
                if (sizeof(_Chr) == 1)
                { alreadyprocessed = _clicklinkfunctor(this, string::convert<wchar_t, _Chr>(plink->item.szUrl)); }
                else
                { alreadyprocessed = _clicklinkfunctor(this, std::basic_string<_Chr>(reinterpret_cast<_Chr *>(plink->item.szUrl))); } 
              }
              if (*plink->item.szUrl && !alreadyprocessed)
              { ShellExecuteW(NULL, L"open", plink->item.szUrl, NULL, NULL, SW_SHOWNORMAL); }
            }
          }
          break;
      }
      return subclassed_control<_Chr, basic_syslink<_Chr> >::wndproc(msg, wparam, lparam);
    }


    virtual SIZE requestsize()
    { return this->sizeofwindowtext(); }
  };

  CTRL_TYPEDEFS(syslink)

}