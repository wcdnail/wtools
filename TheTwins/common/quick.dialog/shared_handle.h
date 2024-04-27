#pragma once
  class null_deleter{ public: void operator()(void *handle) const { } };

	class delete_object
	{	
    private:
      bool _throwerror;
    public:
	    delete_object(bool throwerroronfail = true) 
        : _throwerror(throwerroronfail)
      {	}

	    void operator()(void *handle) const  
      {	
        if (handle)
        { 
          if (!DeleteObject(reinterpret_cast<HGDIOBJ>(handle)) && _throwerror)
          { throw rottedfrog::quickdialog::windowsexception(GetLastError()); }
        }
      }
	};


  //Deleter for shared_handle and unique_handle that calls ReleaseDC for a DC that has been greated by GetDC or CreateDC.
  class release_dc
  {
  private:
      const HWND _hwnd;
  public:
      release_dc(const HWND hwnd): _hwnd(hwnd) { }

      void operator()(const HDC dc)
      { ReleaseDC(_hwnd, dc); }

      void operator()(void *dc)
      { ReleaseDC(_hwnd, reinterpret_cast<HDC>(dc)); }
    
      HWND const & getwnd() const
      { return _hwnd; }
  };

  //Default deleter for unique_handle and shared_handle that calls CloseHandle to close a handle.
	class handle_close
	{	
    public:
	    handle_close() {	}
	    handle_close(const handle_close&) {	}
	    void operator()(void *handle) const  
      {	
        if (handle)
        { 
          if (!CloseHandle(reinterpret_cast<HANDLE>(handle)))
          { throw rottedfrog::quickdialog::handle_exception("CloseHandle failed"); }
        }
      }
	};

  //A non-copyable, non-assignable handle that is automatically closed when it goes out of scope.
  template<class T, class _Dx = handle_close>
  class unique_handle
  {
    private:
      #if defined(__CLANG__) || (defined(_MSC_VER) && _MSC_VER < 1600) // VS 2008 and CLang
        std::tr1::shared_ptr<void> ptr;
      #else
        std::unique_ptr<void, _Dx> ptr;
      #endif
      unique_handle(const unique_handle &);
      unique_handle &operator=(const unique_handle &);
    public:
      T get() const
      { return reinterpret_cast<T>(ptr.get()); }
      
      T release()
      { return reinterpret_cast<T>(ptr.release()); }

      void reset(T handle = T())
      { ptr.reset(handle); }

      unique_handle(T handle)
        : ptr(reinterpret_cast<void *>(handle))
      { }

     template<class D>
     unique_handle(T handle, D deleter)
        : ptr(reinterpret_cast<void *>(handle), deleter)
      { }
  };


  template<class _Ty>
  class shared_handle
  {
    private:
      std::tr1::shared_ptr<void> ptr;
    public:
      _Ty get()
      { return reinterpret_cast<_Ty>(ptr.get()); }
      
      shared_handle(_Ty handle)
        : ptr(reinterpret_cast<void *>(handle))
      { }

      template<class D>
      shared_handle(_Ty handle, D deleter)
        : ptr(reinterpret_cast<void *>(handle), deleter)
      { }
  };