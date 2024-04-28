#pragma once

namespace quickdialog
{
  class image : public subclassed_control<char, image>
  {
    private:
      shared_handle<HICON> _icon;
      shared_handle<HBITMAP> _bitmap;
    public:
      image(shared_handle<HICON> icon)
        : subclassed_control<char, image>(SS_ICON | SS_CENTERIMAGE, WS_EX_TRANSPARENT, "STATIC", ""),
        _icon(icon), _bitmap(NULL, null_deleter())
      { }

      // Note: we supress any errors on handle deletion by passing false when creating the delete_object
      // function object. This is because DeleteObject fails for stock icons. 
      // We can't determine whether something is a stock icon, either from the handle or the error return
      // so we have to supress the error. The only other way would be for the user to know all obout this
      // and pass in the icon as a shared_handle and use the null_deleter, but this breaks encapsulation,
      // so I don't like it that much.
      image(HICON icon)
        : subclassed_control<char, image>(SS_ICON | SS_CENTERIMAGE, WS_EX_TRANSPARENT, "STATIC", ""),
        _icon(icon, delete_object(false)), _bitmap(NULL, null_deleter())
      { }

      image(HBITMAP bitmap)
        : subclassed_control<char, image>(SS_BITMAP | SS_CENTERIMAGE, WS_EX_TRANSPARENT, "STATIC", ""),
          _bitmap(bitmap, delete_object()), _icon(NULL, null_deleter())
      { }

      image(shared_handle<HBITMAP> bitmap)
        : subclassed_control<char, image>(SS_BITMAP | SS_CENTERIMAGE, WS_EX_TRANSPARENT, "STATIC", ""),
          _bitmap(bitmap), _icon(NULL, null_deleter())
      { }

      virtual void init(abstractwindow &wnd)
      {
        if (this->_hwnd ) { return; }
        subclassed_control<char, image>::init(wnd);
        if (_icon.get())
        { win::sendmessage<char>(this->_hwnd, STM_SETICON, (WPARAM)_icon.get(), 0); }
        else if (_bitmap.get())
        { win::sendmessage<char>(this->_hwnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)_bitmap.get()); }
      }

      SIZE requestsize()
      {
        BITMAP b = {};
        if (_icon.get())
        { 
          ICONINFO ii; 
          GetIconInfo( _icon.get(), &ii);
          GetObject(ii.hbmColor, sizeof(b), &b);
        }
        else if (_bitmap.get())
        { GetObject(_bitmap.get(), sizeof(b), &b); }
        SIZE s = { b.bmWidth, b.bmHeight };
        return s;
      }
  };
}