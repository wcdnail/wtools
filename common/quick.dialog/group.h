#pragma once

namespace quickdialog
{
  class group : public aligned_control_base<group>
  {
  private:
    ctrlvector controls;
    bool _uniform;
    SIZE _minsize;
    int _uniformwidth;
  public:
    template<class T> group &operator|(const T &ctrl)  { return controls.push_back(ctrlptr(new T(ctrl))), *this; }
    group &operator|(const char *ctrl) { return controls.push_back(ctrlptr(new paragraph(ctrl))), *this; }
    group &operator|(const wchar_t *ctrl) { return controls.push_back(ctrlptr(new wparagraph(ctrl))), *this; }
    group &operator~() { return _uniform = true, *this; }
    
    group(unsigned int minwidth = 0, unsigned int minheight = 0)
      : _uniform(false)
    { 
      _minsize.cx = minwidth;
      _minsize.cy = minheight;
    }

    bool iscontainer()
    { return controls.size() && controls.back().get()->iscontainer(); }

    virtual void init(abstractwindow &wnd)
    {
     controlbase::init(wnd);
     for (ctrlvector::iterator it = controls.begin(); it != controls.end(); ++it)
     { (*it)->init(wnd); }
    }

    virtual SIZE requestsize()
    {
      SIZE result = {};
      unsigned int marginwidth = 0;
      _uniformwidth = _minsize.cx;
      for (ctrlvector::iterator it = controls.begin(); it != controls.end(); ++it)
      {
        SIZE s = (*it)->requestsize();
        const RECT &margins = (*it)->getmargins();
        marginwidth += margins.left + margins.right;
        s.cy = max(s.cy, _minsize.cy);
        s.cy += margins.top + margins.bottom;
        result.cy = max(s.cy, result.cy);
        if (_uniform)
        { _uniformwidth = max(_uniformwidth, s.cx); }
        else
        { result.cx += max(s.cx, _minsize.cx); }
      }
      if (_uniform)
      { result.cx = _uniformwidth * (controls.size() & 0xFFFFFFFF); }
      result.cx += marginwidth;
      return result;
    }

    virtual void addtowindow(abstractwindow &wnd, int x, int y, unsigned int w, unsigned int h)
    {
      for (ctrlvector::iterator it = controls.begin(); it != controls.end(); ++it)
      {
        SIZE s = (*it)->requestsize();
        const RECT &margins = (*it)->getmargins();
        if (_uniform)
        { s.cx = _uniformwidth; }
        else
        { s.cx = max(s.cx, _minsize.cx); }
        //HACK: sort alignment out properly for container controls.
        if (it != controls.begin())
        { x += margins.left; }
        y += margins.top;
        s.cy = max(s.cy, _minsize.cy);
        (*it)->addtowindow(wnd, x, y, min(static_cast<unsigned int>(s.cx), w), h - (margins.top + margins.bottom));
        x += s.cx + margins.right;
        y -= margins.top;
        w -= s.cx + margins.left + margins.right;
      }
    }

    virtual Alignment getalignment() const
    { return _align; }
  };

  template<> inline group &group::operator|(const std::string &ctrl) { return controls.push_back(ctrlptr(new paragraph(ctrl))), *this; }
  template<> inline group &group::operator|(const std::wstring &ctrl) { return controls.push_back(ctrlptr(new wparagraph(ctrl))), *this; }
}