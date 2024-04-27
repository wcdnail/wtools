#pragma once

namespace quickdialog
{

  enum dialogresult
    {
      qdNone = -1,
      qdCancel = 0,
      qdOK = 1,
      qdYes = 2,
      qdNo = 3,
      qdAbort = 4,
      qdRetry = 5,
      qdContinue = 6,
      qdClose = 7,
      qdHelp = 8,
      qdOther = 9
    };

  class abstractdialogwindow;

  class abstractwindow
  {
    public:
      virtual HWND gethwnd() = 0;
      virtual shared_handle<HDC> getDC() = 0;
      virtual abstractdialogwindow &getparentdialog() = 0;
      virtual void reflectnotify(HWND hwnd) = 0;
      virtual void reflectcommand(HWND hwnd) = 0;
  };

  class abstractdialogwindow : public abstractwindow
  {
    protected:
      HWND _defaultbutton, _cancelbutton;
    public:
      virtual int showdialog(HWND owner = NULL) = 0;
      virtual const RECT &getpadding() = 0;
      virtual shared_handle<HFONT> getfont() = 0;
      virtual void reposition(int x, int y, int w, int h) = 0;
      void reworklayout(int minwidth);
      virtual void setdefaultbutton(HWND hwnd) { _defaultbutton = hwnd; }
      virtual void setcancelbutton(HWND hwnd) { _cancelbutton = hwnd; }
      int _returnvalue;
    
      virtual abstractdialogwindow &getparentdialog()
      { return *this; }

      abstractdialogwindow()
        : _defaultbutton(NULL), _cancelbutton(NULL)
      { }
  };
}