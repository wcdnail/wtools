#pragma once

#include <string>

namespace Twins
{
    class Panel;
    class PanelView;

    void SetCommandLinePath(std::wstring const& newpath);
    void UpdateCommandLinePath();
    bool IsHiddenFilesVisible();
    bool IsPanelActive(Panel const& panel);
    bool IsPanelActive(int id);
    Panel& ActivePanel();
    Panel& InactivePanel();
    bool IsPanelActive(PanelView const& view);
    int PanelIndex(int n);
    void SetActivePanel(int n);
    void SetActivePanel(Panel const& panel);
    void ToggleHiddenVisible();
    void ShowLeftDriveComdo();
    void ShowRightDriveComdo();
    void SwitchPanels();
    void RenameEntry();
    void ViewEntries();
    void EditEntry();
    void CreateEntry();
    void CopyEntries();
    void MoveEntries();
    void CreateDirEntry();
    void RunTerminal();
    void MoveToTrash();
    void MoveToTrash2();
    void DeleteForever();
    void StartPathEdit();
    void TrackHistoryPopup();
    void AddCurrentPathToBookmarks();
    void XorSelection();
    void AddSelection();
    void SubstractSelection();
    void EnterItem();
    void ShowEntryProperties();
    void SelectInsertToggle();
    void MoveCursorUp();
    void MoveCursorDown();
    void MoveCursorUpAndSelect();
    void MoveCursorDownAndSelect();
    void MoveCursorHome();
    void MoveCursorEnd();
    void MoveCursorPageUp();
    void MoveCursorPageDown();
    void ActivateCommandLine();
    void DropdownCommandLine();
    void CancelEditCommandLine(WTL::CComboBoxEx&);
    void PutPathToCommandLine();
    void PutHotNameToCommandLine();
    void FindRequest();
    void CalcDirSize();
    void CalcAllDirSizes();
    void ShowAppAbout();
    void ShowConfigDialog();
    void RestartAppFrame();
    void ExitApplication();
}
