#pragma once

namespace quickdialog
{
  class handle_exception : public std::exception
  {
    private:
      char *_error;
    public:
      handle_exception(const char *error)
        : _error(::_strdup(error))
      { }
      
      virtual char *what()
      { return _error; }
  };

  class windowsexception : public std::exception
  {
    private:
      windowsexception();
      DWORD _errcode;
      char *_message;
    public:
      windowsexception(DWORD errcode)
        : _errcode(errcode), _message(NULL)
      {
#ifdef DEBUG
        MessageBoxA(NULL, what(), "Windows Exception", 0);
#endif
      }

      virtual char *what()
      {
        if (!_message)
        { FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, _errcode, 0, (LPSTR)&_message, 0, NULL); }
        return _message;
      }

      virtual ~windowsexception() throw() {  if (_message) { LocalFree(_message); _message = 0; } }

  };
}