#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <wtypes.h>
#include <atluser.h>
#include <atlctrls.h>

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
                int num;                // number
                DWORD bitnum;           // bit number (0..31)
                std::wstring path;      // `c:\` etc...
                std::wstring label;     // volume label
                std::wstring fs;        // file system name
                UINT type;              // drive type
                DWORD serial;           // drive media
                DWORD length;           // maximal file name length
                DWORD flags;            // file system flags
                mutable bool ready;     // steady flag
                CIconHandle icon;
                int iconIndex;

                Item();
                ~Item();
            };

            typedef std::vector<Item> ItemVector;
            typedef boost::function<void(ItemVector const&)> EnumDoneCallback;

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

            HANDLE enumerate_;
            HANDLE break_;
            boost::thread thread_;
            ItemVector items_;
            mutable boost::mutex itemsMx_;
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
