#pragma once

namespace quickdialog
{
  template<class _Base>
  class container_operators
  {
    protected:
      std::vector<ctrlptr> _controls;
    public:
      template<class T>
      _Base &operator<<(const T &ctrl)
      {
        //C++ 0x only
      #if !defined(_MSC_VER) || _MSC_VER >= 1600
        static_assert(std::is_base_of<abstractcontrol, T>::value, "dialog/containercontrol arguments must be strings or descendants of quickdialog::abstractcontrol");
      #endif
        _controls.push_back(ctrlptr(new T(ctrl)));      
        return (_Base &)*this;
      }

      static bool syslinkenabled()
      {
        static bool enabled = false, check = true;
        if (check)
        {
          INITCOMMONCONTROLSEX iccx = { sizeof(iccx), ICC_LINK_CLASS };
          enabled = InitCommonControlsEx(&iccx) != 0;
          check = false;
        }
        return enabled;
      }

      template<typename _Chr> ctrlptr stringctrlchooser(const std::basic_string<_Chr> &str)
      {
        std::tr1::basic_regex<_Chr> rx(LITERAL(_Chr, ".*\\<[aA] .*\\>.*\\</[aA]\\>.*"));
        if (std::tr1::regex_match(str, rx) && syslinkenabled())
        { return ctrlptr(new basic_syslink<_Chr>(str)); }
        else
        { return ctrlptr(new basic_paragraph<_Chr>(str)); }
      }

      _Base &operator<<(const char *str) { _controls.push_back(stringctrlchooser(std::string(str))); return (_Base &)*this; }
      _Base &operator<<(const wchar_t *str) { _controls.push_back(stringctrlchooser(std::wstring(str))); return (_Base &)*this; }
      _Base &operator<<(const std::string &str) { _controls.push_back(stringctrlchooser(str)); return (_Base &)*this; }
      _Base &operator<<(const std::wstring &str) { _controls.push_back(stringctrlchooser(str)); return (_Base &)*this; }
      _Base &operator<<(const std::vector<ctrlptr> &ctrlvector) { _controls.assign(ctrlvector.begin(), ctrlvector.end()); return (_Base &)*this; }
  };
}