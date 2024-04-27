#pragma once

#include "extern.item.h"
#include "file.viewer.h"
#include "command.line.h"
#include "dialogz.find.h"
#include <settings.h>
#include <history.deque.h>
#include <shell.imagelist.h>
#include <string>
#include <memory>
#include <boost/noncopyable.hpp>
#include <atlstr.h>
#include <atluser.h>
#include <atlwin.h>

namespace Obsolete
{
    using namespace Twins;

    class MainFrame;
    class TabBar;
    class Panel;
    class PanelView;
    class AppConfigDialog;

    class AppState: boost::noncopyable
    {
    public:
        AppState(MainFrame& mainFrame);
        ~AppState();

        bool& IsFirstRun() const;
        HWND GetMainframeHwnd() const;
        void OnThemeChanged();
        void OnPrecreateCreate(CWindow& mainframe);
        void OnCreateViews(HWND parent, CRect& rcLeft, CRect& rcRight);
        void OnDestroy(CWindow& mainframe);
        void OnActivate(unsigned state, bool minimized);
        void OnKeyDown(unsigned code, unsigned flags) const;
        void SetupButtonBar(TabBar& bbar);
        CIcon& MainWindowIcon();
        bool& IsMainWindowMaximized();
        CRect& MainWindowRect();
        bool GetShowHidden() const;
        bool& GetShowHidden();
        Panel& GetLeftPanel();
        Panel& GetRightPanel();
        int GetActivePanelId() const;
        Panel& GetPanelById(int panelId);
        Panel& GetActivePanel();
        Panel& GetInactivePanel();
        void SetActivePanel(int panelId);
        void SwitchPanel();
        void ToggleHidden();
        void DropdownDriveComdo(int panelId);
        void RunTerminal();
        void CreateDirectoryRequest();
        void RenameRequest();
        void CopyRequest();
        void MoveRequest();
        void MoveToTrash();
        void Delete();
        void ShowEntryProperties();
        void TrackHistoryPopup();
        void StartPathEdit();
        void AddCurrentPathToBookmarks();
        void XorSelection();
        void AddSelection();
        void SubstractSelection();
        void EnterItem();
        void SelectInsertToggle();
        void MoveCursorUp();
        void MoveCursorDown();
        void MoveCursorUpAndSelect();
        void MoveCursorDownAndSelect();
        void MoveCursorHome();
        void MoveCursorEnd();
        void MoveCursorPageUp();
        void MoveCursorPageDown();
        void ViewEntries();
        void EditEntry();
        void CreateEntry();
        bool OnEnterCommandLine(CString const& rawLine);
        Ui::CommandLine& GetCommandLine();
        void UpdateCommandLinePath();
        void OnCommandLineCancel(WTL::CComboBoxEx& combo);
        void ActivateCommandLine();
        void DropdownCommandLine();
        void PutPathToCommandLine();
        void PutHotNameToCommandLine();
        void FindRequest();
        void CalcDirSizeRequest(bool multiple);
        void ViewFiles(Fl::List const& files, HWND parent);
        void ExitRequest();
        void AboutRequest();
        void ShowConfigDialog();
        void SetLocale(unsigned id);
        unsigned GetLanguageId() const;
        void RestartMainframeRequest();

    private:
        MainFrame& MainFrameRef;
        unsigned LanguageId;
        std::auto_ptr<Panel> LPanel;
        std::auto_ptr<Panel> RPanel;
        CIcon Icon;
        bool IsMainframeMaximized;
        CRect MainRect;
        CRect ViewerRect;
        bool ShowHidden;
        int ActivePanelIndex;
        Extern::Item ExtrnTerminal;
        Extern::Item ExtrnAkelpad;
        Extern::Item ExtrnNotepad;
        int ButtonBarIds[8];
        FileViewer Viewer;
        Ui::CommandLine Commandline;
        Sh::Imagelist ShellIcons;
        Sh::Imagelist ShellSmallIcons;
        FindDialog::Config FindDialogConfig;
        Conf::Section Options;
        
        static bool ThisRunIsFirst;

        int OnButtonsCommand(TabBar& buttons, int id, PCTSTR text);
        void RunEditor(Panel& panel, std::wstring const& pathname);
        void OnConfigurationCahnged(AppConfigDialog& configDlg);
    };

    inline bool& AppState::IsFirstRun() const { return ThisRunIsFirst; }
    inline unsigned AppState::GetLanguageId() const { return LanguageId; }
    inline CIcon& AppState::MainWindowIcon() { return Icon; }
    inline bool& AppState::IsMainWindowMaximized() { return IsMainframeMaximized; }
    inline CRect& AppState::MainWindowRect() { return MainRect; }
    inline bool AppState::GetShowHidden() const { return ShowHidden; }
    inline bool& AppState::GetShowHidden() { return ShowHidden; }
    inline int AppState::GetActivePanelId() const { return ActivePanelIndex; }
    inline Ui::CommandLine& AppState::GetCommandLine() { return Commandline; }
}
