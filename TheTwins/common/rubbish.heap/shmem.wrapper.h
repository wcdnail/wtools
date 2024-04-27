/**
 *  Re-factored version of Marko Bozikovic's `Console2's` `SharedMemory.h`
 *  Nikonov. M. 2012.
 *  
 */
#pragma once

#include <string>
#include <exception>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/mem_fn.hpp>
#include <accctrl.h>
#include <aclapi.h>

namespace Shelltastix
{
    namespace Shmem
    {
        typedef std::wstring String;

        class Exception: public std::exception
        {
        public:
            Exception(HRESULT code, PCWSTR text, ...);
            virtual ~Exception();

            virtual char const* what() const;
            HRESULT hresult() const;

        private:
            String message_;
            mutable std::string message2_;
            HRESULT code_;
        };

        typedef boost::shared_ptr<SECURITY_ATTRIBUTES> SecurityAttributesPtr;
        typedef boost::shared_ptr<void> HandlePtr;
        typedef boost::shared_ptr<ACL> AclPtr;

        HRESULT InitSecurity(String const& user, 
            SecurityAttributesPtr& sa, 
            HandlePtr& sid, 
            AclPtr& acl, 
            HandlePtr& desc);

        enum 
        {
            SyncObjectsNone = 0,
            SyncObjectsRequest = 1,
            SyncObjectsBoth = 2
        };

        void CreateSyncObjects(SecurityAttributesPtr const& sa, int flags, String const& name,
            HandlePtr& mx, HandlePtr& evReq, HandlePtr& evRsp);

        template <typename T>
        class Wrapper
        {
        public:
            typedef boost::shared_ptr<T> Pointer;

            Wrapper();
            ~Wrapper();

            bool Create(String const& name, DWORD size, int flags = SyncObjectsNone, String const& user = L"");
            bool Open(String const& name, int flags = SyncObjectsNone);

            void Lock();
            void Release();
            void SetReqEvent();
            void SetRespEvent();

            T* Get() const;
            HANDLE GetHandle() const;
            HANDLE GetReqEvent() const;
            HANDLE GetRespEvent() const;

            T& operator[](size_t index) const;
            T* operator->() const;
            T& operator*() const;

            Wrapper& operator = (const T& val);

        private:
            String name_;
            DWORD size_;
            HandlePtr handle_;
            Pointer pointer_;
            HandlePtr mx_;
            HandlePtr evReq_;
            HandlePtr evRsp_;
        };

        class AutoLock: boost::noncopyable
        {
        public:
            template <typename T> 
            explicit AutoLock(Wrapper<T>& shmem)
                : lock_((shmem.Lock(), &shmem), boost::mem_fn(&Wrapper<T>::Release))
            {}

            ~AutoLock();

        private:
            boost::shared_ptr<void> lock_;
        };

        template <typename T>
        inline Wrapper<T>::Wrapper()
            : name_()
            , size_(0)
            , handle_()
            , pointer_()
            , mx_()
            , evReq_()
            , evRsp_()
        {
        }

        template<typename T>
        inline Wrapper<T>::~Wrapper()
        {}

        template <typename T>
        inline bool Wrapper<T>::Create(String const& name, DWORD size, int flags, String const& user)
        {
            SecurityAttributesPtr sattributes;
            HandlePtr sid;
            AclPtr acl;
            HandlePtr sdescriptor;

            HRESULT hr = InitSecurity(user, sattributes, sid, acl, sdescriptor);
            if (S_OK != hr)
                throw Exception(hr, L"InitSecurity(`%s`) failed", user.c_str());

            HandlePtr handle(
                ::CreateFileMapping(INVALID_HANDLE_VALUE, sattributes.get(), PAGE_READWRITE, 0, size * sizeof(T), name.c_str()), 
                ::CloseHandle);

            if (NULL == handle.get())
            {
                HRESULT hr = ::GetLastError();
                throw Exception(hr, 
                    L"CreateFileMapping(`%s`, %d, %d, `%s`) failed", 
                    name.c_str(), size, flags, user.c_str()
                    );
            }

            Pointer pointer(
                static_cast<T*>(::MapViewOfFile(handle.get(), FILE_MAP_ALL_ACCESS, 0, 0, 0)), 
                ::UnmapViewOfFile);

            if (NULL == pointer.get())
            {
                HRESULT hr = ::GetLastError();
                throw Exception(hr, L"MapViewOfFile() failed");
            }

            if (flags > SyncObjectsNone) 
                CreateSyncObjects(sattributes, flags, name, mx_, evReq_, evRsp_);

            ::ZeroMemory(pointer.get(), size * sizeof(T));

            name_ = name;
            size_ = size;
            handle_ = handle;
            pointer_ = pointer;

            return true;
        }

        template <typename T>
        inline bool Wrapper<T>::Open(String const& name, int flags)
        {
            HandlePtr handle(
                ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name.c_str()),
                ::CloseHandle);

            if (!handle || (INVALID_HANDLE_VALUE == handle.get())) 
            {
                HRESULT hr = ::GetLastError();
                throw Exception(hr, L"OpenFileMapping(`%s`) failed", name.c_str());
            }

            Pointer pointer(
                static_cast<T*>(::MapViewOfFile(handle.get(), FILE_MAP_ALL_ACCESS, 0, 0, 0)), 
                ::UnmapViewOfFile);

            if (!pointer_)
            {
                HRESULT hr = ::GetLastError();
                throw Exception(hr, L"MapViewOfFile() failed");
            }

            if (flags > SyncObjectsNone)
                CreateSyncObjects(SecurityAttributesPtr(), flags, name, mx_, evReq_, evRsp_);

            name_ = name;
            size_ = size;
            handle_ = handle;
            pointer_ = pointer;

            return true;
        }

        template<typename T>
        inline void Wrapper<T>::Lock()
        {
            if (mx_) 
                ::WaitForSingleObject(mx_.get(), INFINITE);
        }

        template<typename T>
        inline void Wrapper<T>::Release()
        {
            if (mx_) 
                ::ReleaseMutex(mx_.get());
        }

        template<typename T>
        inline void Wrapper<T>::SetReqEvent()
        {
            if (evReq_) 
                ::SetEvent(evReq_.get());
        }

        template<typename T>
        inline void Wrapper<T>::SetRespEvent()
        {
            if (evRsp_)
                ::SetEvent(evRsp_.get());
        }

        template<typename T>
        inline T* Wrapper<T>::Get() const
        {
            return pointer_.get();
        }

        template<typename T>
        inline HANDLE Wrapper<T>::GetHandle() const
        {
            return handle_.get();
        }

        template<typename T>
        inline HANDLE Wrapper<T>::GetReqEvent() const
        {
            return evReq_.get();
        }

        template<typename T>
        inline HANDLE Wrapper<T>::GetRespEvent() const
        {
            return evRsp_.get();
        }

        template<typename T>
        inline T& Wrapper<T>::operator[](size_t index) const
        {
            return *(pointer_.get() + index);
        }

        template<typename T>
        inline T* Wrapper<T>::operator->() const
        {
            return pointer_.get();
        }

        template <typename T>
        inline T& Wrapper<T>::operator*() const
        {
            return *pointer_;
        }

        template<typename T>
        inline Wrapper<T>& Wrapper<T>::operator = (const T& val)
        {
            *pointer_ = val;
            return *this;
        }
    }
}
