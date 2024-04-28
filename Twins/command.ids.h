#pragma once

namespace Twins
{
    namespace Command
    {
        namespace Ids
        {
            enum Int
            {
                Dummy = 0,
                ToggleHidden,
                DropdownLeftDriveCombo,
                DropdownRightDriveCombo,
                SwitchPanel,
                Rename,
                ViewEntries,
                EditEntry,
                Copy,
                Move,
                CreateDirectory,
                DeleteToTrash,
                DeleteToTrash2,
                Delete,
                RunTerminal,
                ShowEntryProperties,
                TrackHistoryPopup,
                StartPathEdit,
                AddCurrentPathToBookmarks,
                XorSelection,        
                AddSelection,        
                SubstractSelection,  
                EnterItem,           
                SelectToggleHotItem, 
                MoveCursorUp,        
                MoveCursorDown,      
                MoveCursorUpAndSelect,      
                MoveCursorDownAndSelect,
                MoveCursorHome,      
                MoveCursorEnd,       
                MoveCursorPageUp,    
                MoveCursorPageDown,
                CreateEntry,
                ActivateCommandLine,
                DropdownCommandLine,
                PutPathToCommandLine,
                PutHotNameToCommandLine,
                Find,
                CalcDirectorySize,
                CalcDirectorySizes,
                Exit,
                AppAbout,
                ChangeAppConfig,
                RestartMainframe,
            };
        }
    }
}