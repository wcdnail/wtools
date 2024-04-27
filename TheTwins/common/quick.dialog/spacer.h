namespace quickdialog
{
  class spacer : public controlbase
  {
    private:
      unsigned int _spacing;
    public:
      spacer(unsigned int spacing = 12)
        : _spacing(spacing)
      { }

      virtual SIZE requestsize() { SIZE s = { 1, _spacing }; return s; }
      virtual const RECT &getmargins() { static RECT margins = { 0, 0, 0, 0 }; return margins; }
  };
}