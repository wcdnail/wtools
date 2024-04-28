#ifndef _CF_windows_popup_hint_h__
#define _CF_windows_popup_hint_h__

#include <vector>
#include <atlstr.h>
#include <atlwin.h>

namespace CF  // Common framework
{
    class PopupHint
    {
    public:
        PopupHint();
        ~PopupHint();

        void AddHint(PCTSTR hintText, unsigned flags = 0, HICON icon = NULL); /* throw (std::bad_alloc) */
        void PopLast();

    private:
        class Hint;
        typedef Hint* HintPtr;
        typedef std::vector<HintPtr> Hints;

        Hints hints_;

        friend class Hint;
        void OnHintDestroy(Hint* hintRawPtr);

    private:
        PopupHint(PopupHint const&);
        PopupHint& operator = (PopupHint const&);
    };
}

#endif // _CF_windows_popup_hint_h__
