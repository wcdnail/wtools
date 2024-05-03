#ifndef aw_wtl_anchors_h__
#define aw_wtl_anchors_h__

#include <atlwin.h>

struct Anchors
{
    enum Flags
    {
        Nothing = 0x00
    ,   MoveX   = 0x01
    ,   SizeX   = 0x02
    ,   MoveY   = 0x04
    ,   SizeY   = 0x08
    ,   Redraw  = 0x10
    ,   All     = MoveX | SizeX | MoveY | SizeY | Redraw
    };
};

// Задаёт массив контролов[Count], "прикрепленных" к окну-родителю.
// Аналог CDialogResize<T>, но, в отличии от него, ни как 
// не модифицирует окно-родитель.
template <size_t Count, class ControlType = ATL::CWindow>
class CAnchorArray
{
public:
    CAnchorArray() throw() : count_(0), parent_(NULL) {}
    ~CAnchorArray() throw() {}

    // Связать с окном-родителем.
    template <class ParentType>
    void LinkWith(ParentType const& parent) throw() 
    {
        parent_ = parent.m_hWnd;
        CRect rc;
        GetWindowRect(parent_, &rc);
        parSz_.SetSize(rc.Width(), rc.Height());
    }

    // Добавить контрол в массив.
    // При переполнении массива ни чего не делать.
    void SetAnchor(ControlType const& ctl, int flags = Anchors::All) throw() 
    {
        if (count_ < Count) 
            item_[count_++].Set(ctl, parent_, flags);
    }

    // Очистить массив.
    void Clear() throw() { count_ = 0; }

    // Вызвать из обработчика WM_SIZE;
    void OnResize(UINT type, CSize sz) const throw()
    {
        if (SIZE_MINIMIZED != type) 
        {
            for (size_t i=0; i<count_; i++) 
                item_[i].Move(sz, parSz_, parent_);
        }
    }

private:
    class Item
    {
    public:
        Item() {}
        ~Item() {}

        void Set(ControlType const& wnd, HWND parent, int flags) 
        {
            ctl_ = wnd;
            flags_ = flags;

            CRect pos;
            if (ctl_.GetWindowRect(&pos)) 
            {
                ::MapWindowPoints(NULL, parent, (LPPOINT)&pos, 2);
                rc_ = pos;
            }
        }

        void Move(CSize const& szNew, CSize const& szOrigin, HWND parent) const
        {
            if (!flags_) return ;

            CRect rcCtl;
            ::GetWindowRect(ctl_, &rcCtl);
            ::MapWindowPoints(NULL, parent, (LPPOINT)&rcCtl, 2);

            if (flags_ & (Anchors::SizeX | Anchors::MoveX))
            {
                rcCtl.right = rc_.right + (szNew.cx - szOrigin.cx);

                if (flags_ & Anchors::MoveX)
                    rcCtl.left = rcCtl.right - rc_.Width();
            }

            if (flags_ & (Anchors::SizeY | Anchors::MoveY))
            {
                rcCtl.bottom = rc_.bottom + (szNew.cy - szOrigin.cy);

                if (flags_ & Anchors::MoveY)
                    rcCtl.top = rcCtl.bottom - rc_.Height();
            }

            if (flags_ & Anchors::Redraw)
                ::InvalidateRect(ctl_, NULL, TRUE);

            ::SetWindowPos(ctl_, NULL
                , rcCtl.left, rcCtl.top, rcCtl.Width(), rcCtl.Height()
                , SWP_NOZORDER | SWP_NOACTIVATE);
        }

    private:
        CRect rc_;
        ControlType ctl_;
        int flags_;
    };

    Item item_[Count];
    size_t count_;
    HWND parent_;
    CSize parSz_;
};

#endif // aw_wtl_anchors_h__
