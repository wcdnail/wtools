#pragma once

namespace quickdialog
{
 class controlbase : public abstractcontrol
 {
    protected:
      HWND _hwnd;
      abstractwindow *_parent;
    public:
      HWND gethwnd()
      { return _hwnd; }
      
      controlbase()
        : _hwnd(NULL), _parent(NULL)
      { }
      virtual abstractdialogwindow &getparentdialog()
      { 
        _ASSERT(_parent != NULL);
        //TODO: throw exception here
        return _parent->getparentdialog();
      }

      virtual shared_handle<HDC> getDC()
      { return shared_handle<HDC>(GetDC(_hwnd), release_dc(_hwnd)); }

      virtual const RECT &getmargins() 
      { 
        static RECT margins = { 2, 2, 2, 2 };
        return margins;
      }

      void reflectnotify(HWND hwnd)
      { }

      void reflectcommand(HWND hwnd)
      { }

      virtual ~controlbase() { }
      virtual void init(abstractwindow &wnd) { _parent = &wnd; }

      virtual SIZE requestsize()
      { SIZE s = { 0, 0 }; return s; }

      virtual void addtowindow(abstractwindow &wnd, int x, int y, unsigned int w, unsigned int h)
      { }

      virtual Alignment getalignment() const
      { return alStretched; }

  };

  class columnbreak : public controlbase
  {
    virtual bool isColumnBreak() { return true; }
  };

  class sectionbreak : public controlbase
  {
    virtual bool isSectionBreak() { return true; }
  };

  // More classes to get round the multiple this pointers when using multiple inheritance...
  template<class _Base>
  class aligned_control_base : public controlbase
  {
    protected:
      Alignment _align;
    public:
      _Base &operator +() { _align = alRight; return *reinterpret_cast<_Base *>(this); }
      _Base &operator -() { _align = alLeft; return *reinterpret_cast<_Base *>(this); }
      _Base &operator *() { _align = alCentre; return *reinterpret_cast<_Base *>(this); }

      aligned_control_base()
        : _align(alStretched)
      { }

      virtual Alignment getalignment() const
      { return _align; }
  };
  
  template<class _Base, class _Group>
  class control : public aligned_control_base<_Base>
  {
    public:
      template<typename T> _Group &operator |(const T &ctrl) { return *(new _Group()) | *reinterpret_cast<_Base *>(this) | ctrl; }
      template<typename T> _Group &operator |(const T *ctrl) { return *(new _Group()) | *reinterpret_cast<_Base *>(this) | ctrl; }
  };

  class group;
}