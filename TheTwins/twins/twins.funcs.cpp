#include "stdafx.h"
#include "twins.h"
#include "twins.funcs.h"
#include "status.bar.ex.h"
#include "status.ui.h"
#include "iconoz.h"
#include "extern.run.h"
#include "shell.helpers.h"
#include <twins.lang.strings.h>
#include <string.utils.error.code.h>
#include <string.utils.env.vars.h>

void _NotImplementedYet(wchar_t const* caption)
{
    Twins::UiStatus().AddFormat(Twins::Icons.Cancel, L"`%s` %s", caption, _LS(StrId_NotImplementedYet));
}

namespace Twins
{
    void ExitApplication() { App().Exit(); }

    void SetCommandLinePath(std::wstring const& newpath) { App().Commandline.SetPath(newpath); }
    void UpdateCommandLinePath() { SetCommandLinePath(ActivePanel().View.Scanner.Path.FullPath().wstring()); }
    bool IsHiddenFilesVisible() { return App().ShowHidden; }

    Panel& ActivePanel() { return App().Panels[PanelIndex(App().ActivePanelId)]; }
    Panel& InactivePanel() { return App().Panels[1 - PanelIndex(App().ActivePanelId)]; }
    bool IsPanelActive(Panel const& panel) { return App().ActivePanelId == panel.Id; }
    bool IsPanelActive(int id) { return App().ActivePanelId == id; }
    bool IsPanelActive(PanelView const& view) { return &ActivePanel().View == &view; }
    void SetActivePanel(Panel const& panel) { SetActivePanel(panel.Id); }

    int PanelIndex(int n)
    {
        if (n < 0)
            n = 0;

        if (n > _countof(App().Panels) - 1)
            n = _countof(App().Panels) - 1;

        return n;
    }

    void SetActivePanel(int n)
    {
        n = PanelIndex(n);

        if (App().ActivePanelId != n)
        {
            App().ActivePanelId = n;

            InactivePanel().Invalidate();
            ActivePanel().Invalidate();
            ActivePanel().SetFocus();
        }

        UpdateCommandLinePath();
    }

    void ToggleHiddenVisible() 
    { 
        App().ShowHidden = !App().ShowHidden; 

        for (size_t i = 0; i<_countof(App().Panels); i++)
            App().Panels[i].Reload(); 
    }

    void ShowLeftDriveComdo() 
    {
        App().Panels[1].Drives.CancelDropdownCombo(); 
        App().Panels[0].Drives.DropdownCombo(); 
    }

    void ShowRightDriveComdo() 
    { 
        App().Panels[0].Drives.CancelDropdownCombo(); 
        App().Panels[1].Drives.DropdownCombo(); 
    }

    void SwitchPanels() { SetActivePanel(1 - App().ActivePanelId); }
    void RenameEntry() { ActivePanel().View.StartLabelEdit(); }
    void ViewEntries() { ActivePanel().RunViewer(); }

    void RunEditor(Panel& panel, std::wstring const& pathname)
    {
        HRESULT hr = Extern::Run(App().ExtrnAkelpad, panel.GetCurrentPath(), pathname);

        if (S_OK != hr)
            hr = Extern::Run(App().ExtrnNotepad, panel.GetCurrentPath(), pathname);

        SetMainframeStatus(hr, LoadShellIcon(pathname), _LS(StrId_Launcheditorfors), pathname.c_str());
    }

    void EditEntry() 
    {
        if (FItem* it = ActivePanel().View.GetHot())
            RunEditor(ActivePanel(), it->GetFilename());
    }

    void CreateEntry() 
    {
        std::wstring pathname = ActivePanel().CreateEntry();

        if (!pathname.empty())
            RunEditor(ActivePanel(), pathname);
    }

    void RunTerminal() 
    {
        HRESULT hr = Extern::Run(App().ExtrnTerminal, ActivePanel().GetCurrentPath(), L"/D /T:70 /K \"TITLE Terminal\"");

        SetMainframeStatus(hr, LoadShellIcon(Env::Expand(App().ExtrnTerminal.Path))
            , _LS(StrId_Launchss), App().ExtrnTerminal.Name.c_str(), 0 == hr ? L"ok" : L"");
    }

    void CreateDirEntry() { ActivePanel().CreateDir(); }
    void StartPathEdit() { ActivePanel().StartPathEdit(); }
    void TrackHistoryPopup() {}
    void AddCurrentPathToBookmarks() {}
    void XorSelection() {}
    void AddSelection() {}
    void SubstractSelection() {}
    void ShowEntryProperties() {}
    void SelectInsertToggle() {}
    void EnterItem() { ActivePanel().View.EnterItem(); }
    void MoveCursorUp() { ActivePanel().View.MoveCursorUpDown(false); }
    void MoveCursorDown() { ActivePanel().View.MoveCursorUpDown(true); }
    void MoveCursorUpAndSelect() { ActivePanel().View.ToogleSelectionAndMoveCursor(false); }
    void MoveCursorDownAndSelect() { ActivePanel().View.ToogleSelectionAndMoveCursor(true); }
    void MoveCursorHome() { ActivePanel().View.MoveCursorHomeEnd(false); }
    void MoveCursorEnd() { ActivePanel().View.MoveCursorHomeEnd(true); }
    void MoveCursorPageUp() { ActivePanel().View.MoveCursorPageUpDown(false); }
    void MoveCursorPageDown() { ActivePanel().View.MoveCursorPageUpDown(true); }
    void ActivateCommandLine() { App().Commandline.SetFocus(); }
    void DropdownCommandLine() { App().Commandline.ShowDropdown(); }
    void CancelEditCommandLine(WTL::CComboBoxEx&) { ActivePanel().SetFocus(); }

    void PutPathToCommandLine() 
    {
        App().Commandline.AddText(ActivePanel().GetCurrentPath(), true); 
        ActivateCommandLine();
    }

    void PutHotNameToCommandLine() {}
    void FindRequest() {}
    void CalcDirSize() {}
    void CalcAllDirSizes() {}
    void ShowAppAbout() {}
    void ShowConfigDialog() {}
    void RestartAppFrame() {}
    void CopyEntries() {}
    void MoveEntries() {}
    void MoveToTrash() {}
    void MoveToTrash2() {}
    void DeleteForever() {}
}
