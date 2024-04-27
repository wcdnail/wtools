#pragma once

namespace quickdialog
{
  enum Alignment { alStretched, alLeft, alRight, alCentre };  

  class abstractcontrol : public abstractwindow
  {  
  private:
    public:
      bool isBreak()
      { return isSectionBreak() || isColumnBreak(); }
      virtual bool isSectionBreak() { return false; }
      virtual bool isColumnBreak() { return false; }
      virtual bool iscontainer() const { return false; }
      virtual void init(abstractwindow &wnd) = 0;
      virtual SIZE requestsize() = 0;
      virtual const RECT &getmargins() = 0;
      virtual void addtowindow(abstractwindow &wnd, int x, int y, unsigned int w, unsigned int h) = 0;
      virtual Alignment getalignment() const = 0;
      virtual bool getvisible() { return true; }
      virtual bool getenabled() { return true; }
  };

#ifdef __GCC__
  typedef std::shared_ptr<abstractcontrol> ctrlptr;
#else
  typedef std::tr1::shared_ptr<abstractcontrol> ctrlptr;
#endif
  typedef std::vector<ctrlptr> ctrlvector;
}
