#include "stdafx.h"
#include "commands.h"
#include "command.ids.h"
#include "command.key.ids.h"
#include "twins.funcs.h"
#include "keyboard.h"
#include <dh.tracing.h>
#include <twins.langs/twins.lang.strings.h>
#include <stdexcept>
#include <sstream>

namespace Twins
{
    namespace Command
    {
        Manager& Manager::Instance()
        {
            static Command::Manager instance;
            return instance;
        }

        Manager::Manager()
            : map_()
        {}

        Manager::~Manager()
        {}

        void Manager::AddCommand(int id, unsigned keyId, std::wstring const& name, CommandHandler const& callback)
        {
            CommandMap::const_iterator it = map_.find(id);

            if (it != map_.end())
                DH::TPrintf("ECommand: %04d(%s) already present.\n", id, name.c_str());

            map_[id] = Definition(id, keyId, name, callback);
        }

        Definition const& Manager::GetCommand(int id) const
        {
            CommandMap::const_iterator it = map_.find(id);

            static Definition dummy;
            return it == map_.end() ? dummy : it->second;
        }

        void Manager::OnKeyDown(unsigned code, unsigned flags) const
        {
            Keyboard::KeyState state(code, flags);

            struct FinfByKeyId
            {
                FinfByKeyId(unsigned id): id_(id) {}

                bool operator() (CommandMap::value_type const& pair) const
                {
                    return id_ == pair.second.Key;
                }

            private:
                unsigned id_;
            };

            unsigned keyId = state.KeyId();
            CommandMap::const_iterator it = std::find_if(map_.begin(), map_.end(), FinfByKeyId(keyId));
            if (it != map_.end())
            {
#ifdef _DEBUG_KEYBOARD
                DH::TPrintf(L"Keyboard: %8x(%s) `%s`\n", keyId, state.KeyIdName().c_str(), it->second.Name.c_str());
#endif
                it->second.Callback();
            }
            else
                state.Dump();
        }

        void Manager::Initialize()
        {
            AddCommand(Ids::Exit,                       KeyId::Alt_X,           _L(StrId_Exit), ExitApplication);

            AddCommand(Ids::ToggleHidden,               KeyId::Ctrl_H,          _L(StrId_Turnonoffdisplayofhiddenfiles), ToggleHiddenVisible);
            AddCommand(Ids::DropdownLeftDriveCombo,     KeyId::Alt_F1,          _L(StrId_Selectleftdisk), ShowLeftDriveComdo);
            AddCommand(Ids::DropdownRightDriveCombo,    KeyId::Alt_F2,          _L(StrId_Selectrightdisk), ShowRightDriveComdo);
            AddCommand(Ids::SwitchPanel,                KeyId::Tab,             _L(StrId_Switchpanel), SwitchPanels);
            AddCommand(Ids::Rename,                     KeyId::F2,              _L(StrId_Rename), RenameEntry);
            AddCommand(Ids::ViewEntries,                KeyId::F3,              _L(StrId_View), ViewEntries);
            AddCommand(Ids::EditEntry,                  KeyId::F4,              _L(StrId_Edit), EditEntry);
            AddCommand(Ids::CreateEntry,                KeyId::Shift_F4,        _L(StrId_Create), CreateEntry);
            AddCommand(Ids::Copy,                       KeyId::F5,              _L(StrId_Copy), CopyEntries);
            AddCommand(Ids::Move,                       KeyId::F6,              _L(StrId_Move), MoveEntries);
            AddCommand(Ids::CreateDirectory,            KeyId::F7,              _L(StrId_Createdir), CreateDirEntry);
            AddCommand(Ids::RunTerminal,                KeyId::F9,              _L(StrId_Terminal), RunTerminal);
            AddCommand(Ids::DeleteToTrash,              KeyId::F8,              _L(StrId_Movetotrash), MoveToTrash);
            AddCommand(Ids::DeleteToTrash2,             KeyId::Del,             _L(StrId_Movetotrash), MoveToTrash2);
            AddCommand(Ids::Delete,                     KeyId::Shift_Del,       _L(StrId_Delete), DeleteForever);
            AddCommand(Ids::StartPathEdit,              KeyId::Alt_Up,          _L(StrId_Editpath), StartPathEdit);
            AddCommand(Ids::TrackHistoryPopup,          KeyId::Alt_Down,        _L(StrId_Showhistory), TrackHistoryPopup);
            AddCommand(Ids::AddCurrentPathToBookmarks,  KeyId::Ctrl_Up,         _L(StrId_Addpathtobookmarks), AddCurrentPathToBookmarks);
            
            AddCommand(Ids::XorSelection,               KeyId::Multiply,        _L(StrId_Invertselection), XorSelection);
            AddCommand(Ids::AddSelection,               KeyId::Plus,            _L(StrId_Appendselection), AddSelection);
            AddCommand(Ids::SubstractSelection,         KeyId::Minus,           _L(StrId_Subtractselection), SubstractSelection);
            AddCommand(Ids::EnterItem,                  KeyId::Enter,           _L(StrId_Openentryundercursor), EnterItem);
            AddCommand(Ids::ShowEntryProperties,        KeyId::Alt_Enter,       _L(StrId_Showproperties), ShowEntryProperties);
            AddCommand(Ids::SelectToggleHotItem,        KeyId::Insert,          _L(StrId_Selectentryundercursor), SelectInsertToggle);
            AddCommand(Ids::MoveCursorUp,               KeyId::Up,              _L(StrId_Cursorup), MoveCursorUp);
            AddCommand(Ids::MoveCursorDown,             KeyId::Down,            _L(StrId_Cursordown), MoveCursorDown);
            AddCommand(Ids::MoveCursorUpAndSelect,      KeyId::Shift_Up,        _L(StrId_Cursorup), MoveCursorUpAndSelect);
            AddCommand(Ids::MoveCursorDownAndSelect,    KeyId::Shift_Down,      _L(StrId_Cursordown), MoveCursorDownAndSelect);
            AddCommand(Ids::MoveCursorHome,             KeyId::Home,            _L(StrId_Cursortobegin), MoveCursorHome);
            AddCommand(Ids::MoveCursorEnd,              KeyId::End,             _L(StrId_Cursortoend), MoveCursorEnd);
            AddCommand(Ids::MoveCursorPageUp,           KeyId::PageUp,          _L(StrId_Cursortopageup), MoveCursorPageUp);
            AddCommand(Ids::MoveCursorPageDown,         KeyId::PageDown,        _L(StrId_Cursortopagedown), MoveCursorPageDown);
            
            AddCommand(Ids::ActivateCommandLine,        KeyId::Right,           _L(StrId_Activatecommandline), ActivateCommandLine);
            AddCommand(Ids::DropdownCommandLine,        KeyId::Alt_F8,          _L(StrId_Showcommandlinehistory), DropdownCommandLine);
            AddCommand(Ids::PutPathToCommandLine,       KeyId::Ctrl_P,          _L(StrId_Currentpathtocommandline), PutPathToCommandLine);
            AddCommand(Ids::PutHotNameToCommandLine,    KeyId::Ctrl_Enter,      _L(StrId_Entryundercursorfilenametocommandlin), PutHotNameToCommandLine);
            
            AddCommand(Ids::Find,                       KeyId::Alt_F7,          _L(StrId_Search), FindRequest);
            
            AddCommand(Ids::CalcDirectorySize,          KeyId::Space,           _L(StrId_Subdirectorysize), CalcDirSize);
            AddCommand(Ids::CalcDirectorySizes,         KeyId::AltShiftEnter,   _L(StrId_Allsubdirectoriessizes), CalcAllDirSizes);
            
            AddCommand(Ids::AppAbout,                   KeyId::No,              _L(StrId_About), ShowAppAbout);
            AddCommand(Ids::ChangeAppConfig,            KeyId::No,              _L(StrId_Options), ShowConfigDialog);
            
            AddCommand(Ids::RestartMainframe,           KeyId::No,              _L(StrId_RestartMainframe), RestartAppFrame);
        }
    }
}
