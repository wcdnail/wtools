#ifndef aw_wtl_statics_h__
#define aw_wtl_statics_h__

#include <atlgdi.h>

template <size_t Count>
class CPrettyText 
{
public:
    CPrettyText() throw()
        : count_(0)
    {}

    ~CPrettyText() throw() {}

    void Beautify(CWindow const& ctl, COLORREF color, HBRUSH brush, HFONT font) throw() 
    {
        if (count_ < Count) 
        {
            Item &it = item_[count_++];
            it.color = color;
            it.brush = brush;
            it.font  = font;
        }
    }

    HBRUSH OnCtlColorStatic(CDCHandle dc, CStatic ctl) const throw() 
    {
        return NULL;
    }

private:
    struct Item 
    {
        COLORREF color;
        CBrush brush;
        CFont font;

        Item(): color(0) {}
    };

    Item item_[Count];
    size_t count_;

};

#endif // aw_wtl_statics_h__
