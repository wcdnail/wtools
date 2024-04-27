//Helper functions for windows
#pragma once

namespace quickdialog
{

  __inline std::exception lasterrorexception()
  {
    return windowsexception(GetLastError());
  }

  __inline void DebugSysMessage()
  {
    wchar_t *buffer;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, (LPWSTR)&buffer, 0, NULL);
    MessageBoxW(NULL, buffer, L"Debug Message", 0);
    LocalFree(buffer);
  }
  
  // compile time switching between L"" and "" for string literals - a relatively clean macro hides the ugly underbelly.
  #define __ULIT(strlit) L ## strlit
  #define LITERAL(strtype, strlit) rottedfrog::quickdialog::string_literal<strtype>::stringliteral(strlit, __ULIT(strlit))

  template<typename _Char>
  class string_literal
  {
    public:
      static _Char stringliteral(const char *str, const wchar_t *wstr)
      { 
        //static_assert(false, "literals must be either char * or wchar_t *"); 
      }
  };

  template<>
  class string_literal<char>
  {
    public:
      static const char *stringliteral(const char *str, const wchar_t *wstr)
      { return str; }
  };

  template<>
  class string_literal<wchar_t>
  {
    public:
      static const wchar_t *stringliteral(const char *str, const wchar_t *wstr)
      { return wstr; }
  };

  class win
  {
    private:
      typedef HRESULT (WINAPI * ENABLETHEMEDIALOGTEXTURE)(HWND, DWORD);
    
    public:
      static HRESULT enablethemedialogtexture(HWND hwnd, DWORD flags)
      {
        ENABLETHEMEDIALOGTEXTURE etdt;

	      if (visualstylesenabled())
        {
		      etdt = (ENABLETHEMEDIALOGTEXTURE)GetProcAddress(GetModuleHandleA("uxtheme.dll"), "EnableThemeDialogTexture");
		      if (etdt)
          { return etdt(hwnd, flags); }
	      }
        return S_OK;
      }

      static bool visualstylesenabled()
      {
        HMODULE hmod = GetModuleHandleA("comctl32.dll");
        DLLGETVERSIONPROC dllgetversion;

	      if (hmod && (dllgetversion = reinterpret_cast<DLLGETVERSIONPROC>(GetProcAddress(hmod, "DllGetVersion"))))
        {
          DLLVERSIONINFO dllver;
			    dllver.cbSize = sizeof(DLLVERSIONINFO);
			    if (SUCCEEDED(dllgetversion(&dllver)))
          { return (dllver.dwMajorVersion >= 6); }
	      }
	      return false;
      }

      static HWND createwindow(DWORD dwExStyle, const char *className, const std::string &windowName, 
                   DWORD dwStyle, int x, int y, 
                   int nWidth, int nHeight, HWND hWndParent, 
                   HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
      { return ::CreateWindowExA(dwExStyle, className, windowName.c_str(), dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam); }

      static HWND createwindow(DWORD dwExStyle, const wchar_t *className, const std::wstring &windowName, 
                   DWORD dwStyle, int x, int y, 
                   int nWidth, int nHeight, HWND hWndParent, 
                   HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
      { return ::CreateWindowExW(dwExStyle, className, windowName.c_str(), dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam); }

      template <typename _Chr>
      static SIZE getsizeoftext(shared_handle<HDC> hdc, const std::basic_string<_Chr> &text)
      {
        RECT rect = {};
        win::drawtext(hdc.get(), text, rect, DT_CALCRECT);
        SIZE s = { rect.right - rect.left, rect.bottom - rect.top };
        return s;
      };

      template<typename _Chr>
      static LRESULT dispatchmessage(LPMSG msg)
      { }

      template<typename _Chr>
      static BOOL postmessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
      { }

      template<typename _Chr>
      static BOOL getmessage(LPMSG msg, HWND hwnd, UINT wmsgfiltermin, UINT wmsgfiltermax)
      { }

      template<typename _Chr>
      static BOOL isdialogmessage(HWND hwnd, LPMSG msg)
      { }

      template<typename _T>
      static LRESULT defwndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
      { }

      template<typename _T>
      static LRESULT defdlgproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
      { }

      template<typename _Chr>
      static void retrievewindowtext(HWND hwnd, std::basic_string<_Chr> &text)
      {
        LRESULT len = win::sendmessage<_Chr>(hwnd, WM_GETTEXTLENGTH, 0, 0) + 1;
        _Chr *buffer = new _Chr[len];
        win::sendmessage<_Chr>(hwnd, WM_GETTEXT, len, (LPARAM)buffer);
        text.assign(buffer);
        delete[] buffer;
      }

      static int drawtext(HDC hdc, const std::string &str, RECT &r, UINT format)
      { return ::DrawTextA(hdc, str.c_str(), str.size() & 0xFFFFFFFF, &r, format); }

      static int drawtext(HDC hdc, const std::wstring &str, RECT &r, UINT format)
      { return ::DrawTextW(hdc, str.c_str(), str.size() & 0xFFFFFFFF, &r, format); }

      static HFONT createfont(int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, const std::string &pszFaceName)
      { return ::CreateFontA( cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName.c_str()); }

      static HFONT createfont(int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, const std::wstring &pszFaceName)
      { return ::CreateFontW( cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName.c_str()); }

      template<typename _Chr> 
      static BOOL systemparametersinfo(UINT uiaction, UINT uiparam, PVOID pvparam, UINT fwinini)
      { }

      static HFONT createfontindirect(LOGFONTA &lf)
      { return ::CreateFontIndirectA(&lf); }
  
      static HFONT createfontindirect(LOGFONTW &lf)
      { return ::CreateFontIndirectW(&lf); }

      static ATOM registerclassex(const WNDCLASSEXA &wndclass)
      { return ::RegisterClassExA(&wndclass); }

      static ATOM registerclassex(const WNDCLASSEXW &wndclass)
      { return ::RegisterClassExW(&wndclass); }

      template<typename _Chr>
      static LRESULT callwindowproc(WNDPROC wndProc, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
      { }

      template<typename _Chr>
      static LRESULT sendmessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
      { }

      template<typename _Chr>
      static LONG getwindowlong(HWND hwnd, int index)
      { }

      template<typename _Chr>
      static LONG_PTR getwindowlongptr(HWND hwnd, int index)
      { }

      template<typename _Chr>
      static LONG_PTR setwindowlongptr(HWND hwnd, int index, LONG_PTR ptr)
      { }
  };

  template<> 
  inline BOOL win::systemparametersinfo<char>(UINT uiaction, UINT uiparam, PVOID pvparam, UINT fwinini)
  { return ::SystemParametersInfoA(uiaction, uiparam, pvparam, fwinini); }

  template<>
  inline BOOL win::systemparametersinfo<wchar_t>(UINT uiaction, UINT uiparam, PVOID pvparam, UINT fwinini)
  { return ::SystemParametersInfoW(uiaction, uiparam, pvparam, fwinini); }

  template<>
  inline LRESULT win::callwindowproc<char>(WNDPROC wndProc, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
  { return ::CallWindowProcA(wndProc, hwnd, msg, wparam, lparam); }

  template<>
  inline LRESULT win::callwindowproc<wchar_t>(WNDPROC wndProc, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
  { return ::CallWindowProcW(wndProc, hwnd, msg, wparam, lparam); }

  template<>
  inline LRESULT win::sendmessage<char>(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
  { return ::SendMessageA(hwnd, msg, wparam, lparam); }

  template<>
  inline LRESULT win::sendmessage<wchar_t>(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
  { return ::SendMessageW(hwnd, msg, wparam, lparam); }
  
  template<>
  inline LRESULT win::dispatchmessage<char>(LPMSG msg)
  { return ::DispatchMessageA(msg); }  

  template<>
  inline LRESULT win::dispatchmessage<wchar_t>(LPMSG msg)
  { return ::DispatchMessageW(msg); }

  template<>
  inline BOOL win::postmessage<char>(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
  { return ::PostMessageA(hwnd, msg, wparam, lparam); }

  template<>
  inline BOOL win::postmessage<wchar_t>(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
  { return ::PostMessageW(hwnd, msg, wparam, lparam); }

  template<>
  inline BOOL win::getmessage<char>(LPMSG msg, HWND hwnd, UINT wmsgfiltermin, UINT wmsgfiltermax)
  { return ::GetMessageA(msg, hwnd, wmsgfiltermin, wmsgfiltermax); }

  template<>
  inline BOOL win::getmessage<wchar_t>(LPMSG msg, HWND hwnd, UINT wmsgfiltermin, UINT wmsgfiltermax)
  { return ::GetMessageW(msg, hwnd, wmsgfiltermin, wmsgfiltermax); }

  template<>
  inline BOOL win::isdialogmessage<char>(HWND hwnd, LPMSG msg)
  { return ::IsDialogMessageA(hwnd, msg); }

  template<>
  inline BOOL win::isdialogmessage<wchar_t>(HWND hwnd, LPMSG msg)
  { return ::IsDialogMessageW(hwnd, msg); }

  template<>
  inline LRESULT win::defwndproc<char>(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
  { return ::DefWindowProcA(hwnd, msg, wparam, lparam); }

  template<>
  inline LRESULT win::defwndproc<wchar_t>(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
  { return ::DefWindowProcW(hwnd, msg, wparam, lparam); }

  template<>
  inline LRESULT win::defdlgproc<char>(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
  { return ::DefDlgProcA(hwnd, msg, wparam, lparam); }

  template<>
  inline LRESULT win::defdlgproc<wchar_t>(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
  { return ::DefDlgProcW(hwnd, msg, wparam, lparam); }

  template<>
  inline LONG_PTR win::setwindowlongptr<char>(HWND hwnd, int index, LONG_PTR ptr)
  { return ::SetWindowLongPtrA(hwnd, index, ptr); }

  template<>
  inline LONG_PTR win::setwindowlongptr<wchar_t>(HWND hwnd, int index, LONG_PTR ptr)
  { return ::SetWindowLongPtrW(hwnd, index, ptr); }

  template<>
  inline LONG win::getwindowlong<char>(HWND hwnd, int index)
  { return ::GetWindowLongA(hwnd, index); }

  template<>
  inline LONG win::getwindowlong<wchar_t>(HWND hwnd, int index)
  { return ::GetWindowLongW(hwnd, index); }

  template<>
  inline LONG_PTR win::getwindowlongptr<char>(HWND hwnd, int index)
  { return ::GetWindowLongPtrA(hwnd, index); }

  template<>
  inline LONG_PTR win::getwindowlongptr<wchar_t>(HWND hwnd, int index)
  { return ::GetWindowLongPtrW(hwnd, index); }

  template<typename _Chr>
  struct winstructs
  { 
    typedef WNDCLASSEX WndClassEx;
    typedef TCITEMA tc_item;
    static const int xTCM_INSERTITEM = TCM_INSERTITEM;
    typedef NONCLIENTMETRICS non_client_metrics;
  };

  template<>
  struct winstructs<char>
  {
    typedef WNDCLASSEXA WndClassEx;
    typedef NONCLIENTMETRICSA non_client_metrics;
    typedef TCITEMA tc_item;
    static const int xTCM_INSERTITEM = TCM_INSERTITEMA;
  };

  template<>
  struct winstructs<wchar_t>
  {
    typedef WNDCLASSEXW WndClassEx;
    typedef TCITEMW tc_item;
    typedef NONCLIENTMETRICSW non_client_metrics;
    static const int xTCM_INSERTITEM = TCM_INSERTITEMW;
};

  
  #define PICKMESSAGE(_Chr, msg) winmsg<_Chr>( msg ## A, msg ## W)
  
  template<typename _Chr>
  inline unsigned int winmsg(unsigned int normal, unsigned int unicode);

  template<>
  inline unsigned int winmsg<char>(unsigned int normal, unsigned int unicode)
  { return normal; }

  template<>
  inline unsigned int winmsg<wchar_t>(unsigned int normal, unsigned int unicode)
  { return unicode; }

  namespace string
  {
    template<typename _Base, typename _Result>
    inline std::basic_string<_Result> convert(const std::basic_string<_Base> &base, unsigned int codepage = 1252)
    { }

    template<>
    inline std::basic_string<char> convert(const std::basic_string<char> &base, unsigned int codepage)
    { return base; }

    template<>
    inline std::basic_string<wchar_t> convert(const std::basic_string<wchar_t> &base, unsigned int codepage)
    { return base; }

    template<>
    inline std::basic_string<wchar_t> convert(const std::basic_string<char> &base, unsigned int codepage)
    { 
      size_t count = 2 * (base.size() + 1); // ensure buffer is always big enough by multiplying by 2.
      wchar_t *buffer = new wchar_t[count];
      ::mbstowcs_s(NULL, buffer, count, base.c_str(), base.size() + 1);
      std::basic_string<wchar_t> b(buffer);
      delete[] buffer;
      return b;
    }

    template<>
    inline std::basic_string<char> convert(const std::basic_string<wchar_t> &base, unsigned int codepage)
    { 
      size_t count = 2 * (base.size() + 1); // ensure buffer is always big enough by multiplying by 2.
      char *buffer = new char[count];
      ::wcstombs_s(NULL, buffer, count, base.c_str(), base.size());
      std::basic_string<char> b(buffer);
      delete[] buffer;
      return b;
    }
  }
}