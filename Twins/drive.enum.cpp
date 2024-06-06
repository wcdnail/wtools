#include "stdafx.h"
#include "drive.enum.h"
#include <twins.langs/twins.lang.strings.h>
#include <dh.tracing.h>
#include <dbt.h>

namespace Twins
{
    namespace Drive
    {
        static int GetDriveBitNum(DWORD driveBits)
        {
            for (DWORD n = 0; n < sizeof(DWORD) * 8; n++)
                if (0 != (driveBits & (1 << n)))
                    return n;

            return -1;
        }

        PCWSTR Type::ToString(UINT type)
        {
            static const WString strings[] = 
            {
              _L(StrId_Unknown)
            , _L(StrId_Unmounted)
            , _L(StrId_Removable)
            , _L(StrId_Hdd)
            , _L(StrId_Sharemount)
            , _L(StrId_Optical)
            , _L(StrId_Ramdisk)
            , _L(StrId_Floppy)
            , _L(StrId_Network)
            , _L(StrId_Plugins)
            };

            if (type < _countof(strings))
                return strings[type].c_str();

            return L"---";
        }

        Enumerator& Enum()
        {
            static Enumerator inst;
            return inst;
        }

        Enumerator::Enumerator()
            :  enumerate_(::CreateEvent(NULL, FALSE, FALSE, NULL))
            ,      break_(::CreateEvent(NULL, FALSE, FALSE, NULL))
            ,     thread_(&Enumerator::ThreadProc, this)
            ,      items_()
            ,    itemsMx_()
            ,      icons_()
            , onEnumDone_()
        {
        }

        Enumerator::~Enumerator()
        {
            Stop();
        }

        void Enumerator::Stop()
        {
            if (thread_.joinable()) {
                ::SetEvent(break_);
                std::this_thread::yield();
                thread_.join();
            }
            ::CloseHandle(break_);
            ::CloseHandle(enumerate_);
        }

        void Enumerator::Acquire()
        {
            ::SetEvent(enumerate_);
        }

        void Enumerator::ThreadProc()
        {
            DH::ScopedThreadLog lg(L"DRIVENUM");
            HANDLE events[] = { enumerate_, break_ };
            for (;;) {
                DWORD const dwSignal{WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE)};
                if (WAIT_OBJECT_0 == dwSignal) {
                    EnumProc();
                }
                else {
                    break;
                }
            }
        }

        bool Enumerator::EnumProc() 
        {
            const DWORD mask = ::GetLogicalDrives();
            if (!mask) {
                return false;
            }
            try {
                // ##TODO: Get maximal drive number from anywhere...
                enum { MaximalDriveNum = sizeof(DWORD) * 8 }; 

                ItemVector temp;
                temp.reserve(MaximalDriveNum);

                // ##TODO: configure drive icons size and type...
                CImageList tempIcons;
                tempIcons.Create(16, 16, ILC_COLOR32, 1, 1);

                int count = 0;
                for (DWORD n=0; n<MaximalDriveNum; n++) {
                    if (0 != (mask & (1 << n))) {
                        DWORD              serial = 0;
                        DWORD              length = 0; // maximal filename length
                        DWORD               flags = 0; // FILE_CASE_PRESERVED_NAMES ...
                        DWORD           drvLetter = _T('A') + (n & 0xff);
                        TCHAR label[MAX_PATH + 1] = { 0 };
                        TCHAR    fs[MAX_PATH + 1] = { 0 };
                        TCHAR             path[4] = { static_cast<TCHAR>(drvLetter), _T(':'), _T('\\'), 0 };

                        const UINT type = ::GetDriveType(path);
                        switch (type) {
                        case DRIVE_UNKNOWN     /*0*/:
                        case DRIVE_NO_ROOT_DIR /*1*/: 
                            // ##TODO: Unmounted
                            continue;
                        case DRIVE_REMOVABLE   /*2*/: 
                            break;  
                            // ##TODO: Determine floppy drive
                        case DRIVE_FIXED       /*3*/:
                        case DRIVE_CDROM       /*5*/:
                        case DRIVE_RAMDISK     /*6*/:
                            ::GetVolumeInformation(path, label, _countof(label)-1, &serial, &length, &flags, fs, _countof(fs));
                            break;
                        case DRIVE_REMOTE      /*4*/: 
                            // ##TODO: ::GetVolumeInformation to slow
                            break;
                        default:
                            continue;
                        }
                        DH::TPrintf(L"DRIVENUM", L"%d %s [%s] (0x%08x %s(%d) 0x%08x)\n",
                            type, path, label, serial, fs, length, flags);
                        Item info;
                        info.num = count++;
                        info.bitnum = n;
                        info.path = path;
                        info.label = label;
                        info.fs = fs;
                        info.type = type;
                        info.serial = serial;
                        info.length = length;
                        info.flags = flags;
                        LoadAndStoreIcon(info, tempIcons);
                        temp.emplace_back(std::move(info));
                    }
                }
                {
                    std::lock_guard lk(itemsMx_);
                    items_.swap(temp);
                    icons_.Destroy();
                    icons_.Attach(tempIcons.Detach());
                }
                if (onEnumDone_) {
                    onEnumDone_(items_);
                }
                return true;
            }
            catch (std::exception const& ex) {
                DH::TPrintf("DRIVENUM", "ERROR <%s>\n", ex.what());
            }
            catch (...) {
                DH::TPrintf(L"DRIVENUM", L"UNKNOWN ERROR\n");
            }
            return false;
        }

        bool Enumerator::IsIndexValid(int driveId) const
        {
            return (driveId >= 0) && (driveId <= (int)items_.size()-1);
        }

        void Enumerator::CheckIndex(int& driveId) const
        {
            if (!IsIndexValid(driveId))
            {
                if (driveId < 0)
                    driveId = 0;

                int count = (int)items_.size()-1;

                if (driveId > count)
                    driveId = count;
            }
        }

        Enumerator::Item const& Enumerator::Get(int driveId) const /* throw(std::out_of_range) */
        {
            if (IsIndexValid(driveId))
                return items_[driveId];

            static Enumerator::Item dummy;
            return dummy;
        }

        CImageList& Enumerator::GetImageList()
        {
            return icons_;
        }

        void Enumerator::LoadAndStoreIcon(Item& item, CImageList& list)
        {
            SHFILEINFO info = {0};

            ::SHGetFileInfo(item.path.c_str(), FILE_ATTRIBUTE_DIRECTORY, &info, sizeof(info), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
// ##TODO: handle `NULL == info.hIcon`"))
            item.iconIndex = list.AddIcon(info.hIcon);
            item.icon.Attach(info.hIcon);
        }

        BOOL Enumerator::ProcessWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD msgMapId /*= 0*/)
        {
            if (WM_DEVICECHANGE == message)
            {
                DWORD eventType = (DWORD)wParam;
                PDEV_BROADCAST_HDR eventHdr = (PDEV_BROADCAST_HDR)lParam;
                PDEV_BROADCAST_VOLUME volumeInfo = (PDEV_BROADCAST_VOLUME)eventHdr;

                bool haveHdr        = NULL != eventHdr;
                DWORD deviceType    = haveHdr ? eventHdr->dbch_devicetype : (DWORD)-1;
                bool haveVolume     = haveHdr ? (DBT_DEVTYP_VOLUME == deviceType) : false;

                switch (eventType)
                {
                case DBT_DEVICEARRIVAL: 
                    OnDriveArrive((haveVolume ? volumeInfo->dbcv_unitmask : 0), (haveVolume ? volumeInfo->dbcv_flags : (WORD)-1));
                    break;

                case DBT_DEVICEREMOVECOMPLETE: 
                    OnDriveRemove((haveVolume ? volumeInfo->dbcv_unitmask : 0), (haveVolume ? volumeInfo->dbcv_flags : (WORD)-1));
                    break;
                }

                lResult = ::DefWindowProc(window, message, wParam, lParam);
                return TRUE;
            }

            return FALSE;
        }

        bool Enumerator::IsDrivePresent(int drive) const
        {
            for (ItemVector::const_iterator it = items_.begin(); it != items_.end(); ++it)
                if (it->bitnum == drive)
                    return true;

            return false;
        }

        void Enumerator::OnDriveArrive(DWORD driveBits, WORD flags)
        {
            Acquire();
        }

        void Enumerator::OnDriveRemove(DWORD driveBits, WORD flags)
        {
            Acquire();
        }

        int Enumerator::FindByName(std::wstring const& name) const
        {
            if (name.empty())
                return -1;

            struct _name_eq
            {
                std::wstring const& name;
                _name_eq(std::wstring const& n): name(n) {}
                bool operator() (Item const& it) { return ::toupper(name[0]) == ::toupper(it.path[0]); }
            };

            ItemVector::const_iterator fi = std::find_if(items_.begin(), items_.end(), _name_eq(name));
            return fi == items_.end() ? -1 : fi->num;
        }
    }
}
