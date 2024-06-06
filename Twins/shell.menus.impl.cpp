#include "stdafx.h"
#include "shell.menus.impl.h"
#include "file.list.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>
#include <comdef.h>

#define MIN_ID 1
#define MAX_ID 10000

namespace Twins
{
    namespace ShellImpl
    {
        static void HandleHresult(HRESULT hr, PCWSTR format, ...)
        {
            if (FAILED(hr)) {
                CStringW message;

                va_list ap;
                va_start(ap, format);
                message.FormatV(format, ap);
                va_end(ap);

                message.AppendFormat(L" %08x (%s)", hr, Str::ErrorCode<>::SystemMessage(hr));
                DH::TPrintf(TL_Error, L"%s\n", message);

                throw _com_error(hr);
            }
        }

        HRESULT SHBindToParentEx(CComPtr<IShellFolder> const& psfDesktop, LPCITEMIDLIST pidl, REFIID riid, VOID **ppv, LPCITEMIDLIST *ppidlLast);
        LPITEMIDLIST CopyPIDL(LPCITEMIDLIST pidl, int cb = -1);
        UINT GetPIDLSize(LPCITEMIDLIST pidl);
        LPBYTE GetPIDLPos(LPCITEMIDLIST pidl, int nPos);
        int GetPIDLCount(LPCITEMIDLIST pidl);
        void InvokeCommand(LPCONTEXTMENU pContextMenu, UINT idCommand);

        static void FreePIDLArray(LPITEMIDLIST*& pidlArray)
        {
            if (!pidlArray)
                return ;

            size_t iSize = _msize (pidlArray) / sizeof (LPITEMIDLIST);
            for (size_t i = 0; i < iSize; i++)
                free(pidlArray[i]);

            free(pidlArray);
            pidlArray = NULL;
        }

        static HRESULT SHBindToParentEx(CComPtr<IShellFolder> const& psfDesktop, LPCITEMIDLIST pidl, REFIID riid, VOID **ppv, LPCITEMIDLIST *ppidlLast)
        {
            HRESULT hr = 0;
            if (!pidl || !ppv)
                return E_POINTER;

            int nCount = GetPIDLCount(pidl);
            if (nCount == 0)	// desktop pidl of invalid pidl
                return E_POINTER;

            if (nCount == 1)	// desktop pidl
            {
                if ((hr = psfDesktop->QueryInterface(riid, ppv)) == S_OK)
                {
                    if (ppidlLast) 
                        *ppidlLast = CopyPIDL(pidl);
                }
                return hr;
            }

            LPBYTE pRel = GetPIDLPos(pidl, nCount - 1);
            LPITEMIDLIST pidlParent = NULL;
            pidlParent = CopyPIDL(pidl, (int)(pRel - (LPBYTE)pidl));

            CComPtr<IShellFolder> psfFolder;
            if ((hr = psfDesktop->BindToObject(pidlParent, NULL, __uuidof (psfFolder), (void **) &psfFolder)) != S_OK)
            {
                free (pidlParent);
                return hr;
            }
            if ((hr = psfFolder->QueryInterface(riid, ppv)) == S_OK)
            {
                if (ppidlLast)
                    *ppidlLast = CopyPIDL((LPCITEMIDLIST) pRel);
            }
            free (pidlParent);
            return hr;
        }

        static LPBYTE GetPIDLPos(LPCITEMIDLIST pidl, int nPos)
        {
            if (!pidl)
                return 0;
            int nCount = 0;

            BYTE * pCur = (BYTE *) pidl;
            while (((LPCITEMIDLIST) pCur)->mkid.cb)
            {
                if (nCount == nPos)
                    return pCur;
                nCount++;
                pCur += ((LPCITEMIDLIST) pCur)->mkid.cb;	// + sizeof(pidl->mkid.cb);
            }
            if (nCount == nPos) 
                return pCur;
            return NULL;
        }

        static int GetPIDLCount(LPCITEMIDLIST pidl)
        {
            if (!pidl)
                return 0;

            int nCount = 0;
            BYTE*  pCur = (BYTE *) pidl;
            while (((LPCITEMIDLIST) pCur)->mkid.cb)
            {
                nCount++;
                pCur += ((LPCITEMIDLIST) pCur)->mkid.cb;
            }
            return nCount;
        }

        ContextMenu::ContextMenu()
            : count_(0)
            , popup_(NULL)
            , ifolder_(NULL)
            , idls_(NULL)
        {
        }

        ContextMenu::~ContextMenu()
        {
            FreePIDLArray(idls_);
        }

        BOOL ContextMenu::GetContextMenu(CComPtr<IContextMenu>& ppContextMenu, int & iMenuType)
        {
            CComPtr<IContextMenu> icm1;
            ifolder_->GetUIObjectOf(NULL, count_, (LPCITEMIDLIST*)idls_, IID_IContextMenu, NULL, (void**)&icm1);
            if (icm1)
            {
                if (icm1->QueryInterface(IID_IContextMenu3, (PVOID*)&ppContextMenu) == NOERROR)
                    iMenuType = 3;
                else if (icm1->QueryInterface(IID_IContextMenu2, (PVOID*)&ppContextMenu) == NOERROR)
                    iMenuType = 2;

                if (!ppContextMenu) 
                {	
                    iMenuType = 1;
                    ppContextMenu.Attach(icm1.Detach());
                }
            }
            else
                return FALSE;

            return TRUE;
        }

        UINT ContextMenu::ShowContextMenu(CWindow owner, CPoint pt)
        {
            UINT cmdId = (UINT)-1;

            try
            {
                int iMenuType = 0;
                CComPtr<IContextMenu> pContextMenu;

                if (!GetContextMenu(pContextMenu, iMenuType))	
                    return 0;

                CMenu tempMenu;
                tempMenu.CreatePopupMenu();
                HRESULT hr = pContextMenu->QueryContextMenu(tempMenu.m_hMenu, 0, MIN_ID, MAX_ID, CMF_NORMAL | CMF_EXPLORE);
                HandleHresult(hr, L"IContextMenu::QueryContextMenu");

                popup_.Attach(tempMenu.Detach());
                cmdId = popup_.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, owner);

                if (cmdId >= MIN_ID && cmdId <= MAX_ID)
                    InvokeCommand(pContextMenu, cmdId - MIN_ID);
            }
            catch (...)
            {}

            return cmdId;
        }

        static void InvokeCommand(LPCONTEXTMENU pContextMenu, UINT idCommand)
        {
            CMINVOKECOMMANDINFO cmi = {0};
            cmi.cbSize = sizeof (CMINVOKECOMMANDINFO);
            cmi.lpVerb = (LPSTR) MAKEINTRESOURCE (idCommand);
            cmi.nShow = SW_SHOWNORMAL;

            pContextMenu->InvokeCommand(&cmi);
        }

        HRESULT ContextMenu::SetObjects(Fl::List const& files)
        {
            if (files.Count() < 1)
                return E_INVALIDARG;

            HRESULT hr = S_FALSE;

            try
            {
                CComPtr<IShellFolder> psfDesktop;
                hr = ::SHGetDesktopFolder(&psfDesktop);
                HandleHresult(hr, L"SHGetDesktopFolder");

                LPITEMIDLIST pidl = NULL;
                CStringW tempString(files[0].GetPath().c_str());
                hr = psfDesktop->ParseDisplayName(NULL, 0, tempString.GetBuffer(), NULL, &pidl, NULL);
                HandleHresult(hr, L"IShellFolder::ParseDisplayName");

                LPITEMIDLIST pidlItem = NULL;	// relative pidl
                CComPtr<IShellFolder> tempFolder;
                hr = SHBindToParentEx(psfDesktop, pidl, IID_IShellFolder, (void **) &tempFolder, NULL);
                HandleHresult(hr, L"SHBindToParentEx");
                free(pidlItem);

                CComPtr<IMalloc> lpMalloc;
                SHGetMalloc (&lpMalloc);
                lpMalloc->Free(pidl);

                int count = files.Count();
                LPITEMIDLIST* pidlArray = (LPITEMIDLIST*)::calloc(count, sizeof(LPITEMIDLIST));
                for (int i = 0; i < count; i++)
                {
                    tempString = files[i].GetPath().c_str();
                    PWSTR name = tempString.GetBuffer();
                    hr = psfDesktop->ParseDisplayName(NULL, 0, name, NULL, &pidl, NULL);
                    HandleHresult(hr, L"IShellFolder::ParseDisplayName(`%s`)", name);

                    CComPtr<IShellFolder> psfFolder;
                    hr = SHBindToParentEx(psfDesktop, pidl, IID_IShellFolder, (void **)&psfFolder, (LPCITEMIDLIST *)&pidlItem);
                    HandleHresult(hr, L"SHBindToParentEx(`%s`)", name);

                    pidlArray[i] = CopyPIDL(pidlItem);
                    free(pidlItem);
                    lpMalloc->Free(pidl);
                }

                count_ = count;
                FreePIDLArray(idls_);
                idls_ = pidlArray;
                ifolder_.Attach(tempFolder.Detach());
            }
            catch (_com_error const& ex)
            {
                hr = ex.Error();
            }

            return hr;
        }

#if 0
        HRESULT ContextMenu::SetObjects(CString strObject)
        {
            StringArray strArray;
            strArray.push_back(strObject);
            return SetObjects(strArray);
        }

        void ContextMenu::SetObjects(LPITEMIDLIST pidl)
        {
            FreePIDLArray(m_pidlArray);

            LPITEMIDLIST pidlItem = NULL;
            SHBindToParent ((LPCITEMIDLIST) pidl, IID_IShellFolder, (void **) &m_psfFolder, (LPCITEMIDLIST *) &pidlItem);	

            m_pidlArray = (LPITEMIDLIST *) malloc (sizeof (LPITEMIDLIST));	// allocate ony for one elemnt
            m_pidlArray[0] = CopyPIDL (pidlItem);

            LPMALLOC lpMalloc = NULL;
            SHGetMalloc (&lpMalloc);
            lpMalloc->Free (pidlItem);
            lpMalloc->Release();

            nItems = 1;
        }

        void ContextMenu::SetObjects(IShellFolder *psfFolder, LPITEMIDLIST pidlItem)
        {
            FreePIDLArray(m_pidlArray);

            m_psfFolder = psfFolder;

            m_pidlArray = (LPITEMIDLIST *) malloc (sizeof (LPITEMIDLIST));
            m_pidlArray[0] = CopyPIDL (pidlItem);

            nItems = 1;
        }

        void ContextMenu::SetObjects(IShellFolder * psfFolder, LPITEMIDLIST *pidlArray, int nItemCount)
        {
            FreePIDLArray(m_pidlArray);

            m_psfFolder = psfFolder;
            m_pidlArray = (LPITEMIDLIST *) malloc (nItemCount * sizeof (LPITEMIDLIST));

            for (int i = 0; i < nItemCount; i++)
                m_pidlArray[i] = CopyPIDL (pidlArray[i]);

            nItems = nItemCount;
        }
#endif // 0

        static LPITEMIDLIST CopyPIDL(LPCITEMIDLIST pidl, int cb)
        {
            if (cb == -1)
                cb = GetPIDLSize (pidl); // Calculate size of list.

            LPITEMIDLIST pidlRet = (LPITEMIDLIST) calloc (cb + sizeof (USHORT), sizeof (BYTE));
            if (pidlRet)
                CopyMemory(pidlRet, pidl, cb);

            return (pidlRet);
        }


        static UINT GetPIDLSize(LPCITEMIDLIST pidl)
        {  
            if (!pidl) 
                return 0;
            int nSize = 0;
            LPITEMIDLIST pidlTemp = (LPITEMIDLIST) pidl;
            while (pidlTemp->mkid.cb)
            {
                nSize += pidlTemp->mkid.cb;
                pidlTemp = (LPITEMIDLIST) (((LPBYTE) pidlTemp) + pidlTemp->mkid.cb);
            }
            return nSize;
        }

        CMenu& ContextMenu::GetMenu()
        {
            return popup_;
        }
    }
}
