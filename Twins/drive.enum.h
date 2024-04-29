#pragma once

#include <wtypes.h>
#include <atluser.h>
#include <atlctrls.h>
#include <string>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>

namespace Twins
{
    namespace Drive
    {
        struct Type
        {
            enum
            {
                Unknown = 0,
                Unmounted,
                Removable,
                Fixed,
                Remote,
                Optical,
                RamDisk,
                Floppy,
                NetworkRoot,
                ExtraRoot,
            };

            static PCWSTR ToString(UINT type);
        };

        class Enumerator
        {
        public:
            struct Item
            {
                int            num = -1;    // number
                DWORD       bitnum = 0;     // bit number (0..31)
                UINT          type = 0;     // drive type
                DWORD       serial = 0;     // drive media
                DWORD       length = 0;     // maximal file name length
                DWORD        flags = 0;     // file system flags
                mutable bool ready = false; // steady flag
                int      iconIndex = 0;
                CIconHandle   icon;
                std::wstring  path;         // `c:\` etc...
                std::wstring label;         // volume label
                std::wstring    fs;         // file system name
            };

            typedef std::vector<Item> ItemVector;
            typedef std::function<void(ItemVector const&)> EnumDoneCallback;

            void Acquire();
            bool IsIndexValid(int driveId) const;
            void CheckIndex(int& driveId) const;
            Item const& Get(int driveId) const; /* throw(std::out_of_range) */
            CImageList& GetImageList();
            EnumDoneCallback& OnEnumDone();
            bool IsDrivePresent(int drive) const;
            int FindByName(std::wstring const& name) const;

            BOOL ProcessWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD msgMapId = 0);

        private:
            friend Enumerator& Enum();

            Enumerator();
            ~Enumerator();
            void Stop();

            HANDLE enumerate_;
            HANDLE break_;
            std::thread thread_;
            ItemVector items_;
            mutable std::mutex itemsMx_;
            CImageList icons_;
            EnumDoneCallback onEnumDone_;

            void ThreadProc();
            bool EnumProc();
            static void LoadAndStoreIcon(Item& item, CImageList& list);

            void OnDriveArrive(DWORD driveBits, WORD flags);
            void OnDriveRemove(DWORD driveBits, WORD flags);

        private:
            Enumerator(Enumerator const&);
            Enumerator& operator = (Enumerator const&);
        };

        inline Enumerator::EnumDoneCallback& Enumerator::OnEnumDone() { return onEnumDone_; }
        
        extern Enumerator& Enum();
    }
}
