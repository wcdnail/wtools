#include "stdafx.h"
#include "twins.state.h"
#include "commands.h"
#include "tab.bar.h"
#include "panel.h"
#include "drive.enum.h"
#include "extern.run.h"
#include "panel.view.selecting.h"
#include "file.operation.generic.h"
#include "file.copy.h"
#include "file.move.h"
#include "file.delete.h"
#include "twins.main.frame.h"
#include "status.ui.h"
#include "dialogz.search.text.h"
#include "dialogz.app.about.h"
#include "dialogz.options.h"
#include "iconoz.h"
#include <string.utils.env.vars.h>
#include <reflection/ref.rect.h>
#include <rect.putinto.h>
#include <rect.screen.h>
#include <twins.langs/twins.lang.strings.h>
#include <dh.tracing.h>
#include "res/resource.h"

void _NotImplementedYet(wchar_t const* caption)
{
    Twins::UiStatus().AddFormat(Twins::Icons.Cancel, L"`%s` %s", caption, _LS(StrId_NotImplementedYet));
}

namespace Obsolete
{
    bool AppState::ThisRunIsFirst = true;

    AppState::AppState(MainFrame& mainFrame)
        : MainFrameRef(mainFrame)
        , LanguageId(MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT))
        , LPanel(NULL)
        , RPanel(NULL)
        , Icon()
        , IsMainframeMaximized(false)
        , MainRect(0, 0, 1000, 640)
        , ViewerRect(0, 0, Rc::Screen.Width() - Rc::Screen.Width()/4, Rc::Screen.Height() - Rc::Screen.Height()/4)
        , ShowHidden(false)
        , ActivePanelIndex(0)
        , ExtrnTerminal(L"Terminal", L"%COMSPEC%")
        , ExtrnAkelpad(L"AkelPad", L"%AKEL_ROOT%\\AkelPad.exe")
        , ExtrnNotepad(L"Notepad", L"notepad")
        , Viewer(ViewerRect)
        , Commandline(std::bind(&AppState::OnEnterCommandLine, this, _1))
        , ShellIcons(false)
        , ShellSmallIcons(true)
        , FindDialogConfig(ShellSmallIcons)
        , Options(::Settings(), L"Common")
    {
        Rc::PutInto(Rc::Screen, MainRect, Rc::Center);
        Rc::PutInto(Rc::Screen, ViewerRect, Rc::Right | Rc::YCenter);

        ButtonBarIds[0] = Command::Ids::Rename;
        ButtonBarIds[1] = Command::Ids::ViewEntries;
        ButtonBarIds[2] = Command::Ids::EditEntry;
        ButtonBarIds[3] = Command::Ids::Copy;
        ButtonBarIds[4] = Command::Ids::Move;
        ButtonBarIds[5] = Command::Ids::CreateDirectory;
        ButtonBarIds[6] = Command::Ids::DeleteToTrash;
        ButtonBarIds[7] = Command::Ids::RunTerminal;

        std::unique_ptr<Panel> tempLPanel = std::make_unique<Panel>(*this, 0);
        std::unique_ptr<Panel> tempRPanel = std::make_unique<Panel>(*this, 1);

        LPanel = std::move(tempLPanel);
        RPanel = std::move(tempRPanel);

        FromSettings(Options, LanguageId);
        FromSettings(Options, IsMainframeMaximized);
        FromSettings(Options, MainRect);
        FromSettings(Options, ViewerRect);
        FromSettings(Options, ShowHidden);
        FromSettings(Options, ActivePanelIndex);
        FromSettings(Options, ButtonBarIds);
        FromSettings(Options, ThisRunIsFirst);

        Strings::Instance().Select(LanguageId);
    }

    AppState::~AppState()
    {}

    HWND AppState::GetMainframeHwnd() const { return MainFrameRef.operator HWND(); }

    void AppState::SetLocale(unsigned id)
    {
        LanguageId = id;
        Strings::Instance().Select(LanguageId);

        unsigned localeId = (LCID)LanguageId;

        wchar_t localeName[256] = {0};
        if (::GetLocaleInfoW(localeId, LOCALE_SLANGUAGE, localeName, _countof(localeName)-1))
        {
            ::SetThreadLocale(localeId);
            ::_wsetlocale(LC_ALL, localeName);
        }
    }

    void AppState::OnThemeChanged()
    {
        LPanel->OnThemeChanged();
        RPanel->OnThemeChanged();
    }

    void AppState::OnPrecreateCreate(CWindow& mainframe)
    {
        mainframe.MoveWindow(MainRect);

        Icon.LoadIcon(IDR_MAINFRAME);
        if (!Icon.IsNull())
        {
            mainframe.SetIcon(Icon, TRUE);
            mainframe.SetIcon(Icon, FALSE);
        }
    }

    void AppState::OnCreateViews(HWND parent, CRect& rcLeft, CRect& rcRight)
    {
        LPanel->Create(parent, rcLeft);
        RPanel->Create(parent, rcRight);

        Twins::Drive::Enum().OnEnumDone() = std::bind(DriveBar::InitializeDrives, 
            std::ref(LPanel->GetDriveBar()), 
            std::ref(RPanel->GetDriveBar()), 
            _1);

        Twins::Drive::Enum().Acquire();

        Commands().Initialize(*this);

        Commandline.GetCancelCallback() = std::bind(&AppState::OnCommandLineCancel, this, _1);
    }


    void AppState::OnDestroy(CWindow& mainFrame)
    {
        IsMainframeMaximized = (TRUE == mainFrame.IsZoomed());
        if (!IsMainframeMaximized)
            mainFrame.GetWindowRect(MainRect);
    }

    void AppState::OnActivate(unsigned state, bool minimized)
    {
        ActivePanel().SetFocus();
    }

    void AppState::OnKeyDown(unsigned code, unsigned flags) const
    {
        Commands().OnKeyDown(code, flags);
    }

    Panel& AppState::GetLeftPanel() { return *LPanel; }
    Panel& AppState::GetRightPanel() { return *RPanel; }
    Panel& AppState::GetPanelById(int panelId) { return *(panelId ? RPanel : LPanel); }
    Panel& AppState::ActivePanel() { return GetPanelById(ActivePanelIndex); }
    Panel& AppState::GetInactivePanel() { return GetPanelById(1 - ActivePanelIndex);  }
    
    void AppState::SetActivePanel(int panelId)
    {
        if (ActivePanelIndex != panelId)
        {
            ActivePanelIndex = panelId;

            GetInactivePanel().Invalidate();
            ActivePanel().Invalidate();
            ActivePanel().SetFocus();
        }

        UpdateCommandLinePath();
    }

    void AppState::SwitchPanel()
    {
        SetActivePanel(1 - ActivePanelIndex);
    }

    void AppState::DropdownDriveComdo(int panelId)
    {
        GetPanelById(1 - panelId).GetDriveBar().CancelDropdownCombo();
        GetPanelById(panelId).GetDriveBar().DropdownCombo();
    }

    void AppState::SetupButtonBar(TabBar& bbar)
    {
        UINT dflags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

        for (size_t i=0; i<_countof(ButtonBarIds); i++)
        {
            Command::Definition const& def = Commands().GetCommand(ButtonBarIds[i]);
            WString name = def.GetKeyIdName() + L" " + def.Name.c_str();
            bbar.Add(name.c_str(), dflags, NULL, ButtonBarIds[i]);
        }

        bbar.OnClick() = std::bind(&AppState::OnButtonsCommand, this, std::ref(bbar), _1, _2);
    }

    int AppState::OnButtonsCommand(TabBar& buttons, int id, PCTSTR text)
    {
        Dh::ThreadPrintf(L"PnButton: %2d `%s`\n", id, text);
        buttons.ClearHot();
        Commands().GetCommand(id).Callback();
        return id;
    }

    void AppState::ToggleHidden()
    {
        ShowHidden = !ShowHidden;
        LPanel->Reload();
        RPanel->Reload();
    }

    void AppState::RunTerminal()
    {
        Extern::Run(ExtrnTerminal, ActivePanel().GetCurrentPath(), L"/D /T:70 /K \"TITLE Terminal\"");
        //UiStatus().SetText(_LS("Запуск '%s' - %s"), ExtrnTerminal.Name.c_str(), (S_OK == hr ? L"ok" : Str::ErrorCode<wchar_t>::SystemMessage(hr)));
    }

    void AppState::ViewFiles(Fl::List const& files, HWND parent)
    {
        Viewer.Show(files, MainFrameRef.GetMessageLoop(), parent);
    }

    void AppState::ViewEntries()
    {
        Fl::List files;
        if (ActivePanel().GetSelection(files))
            ViewFiles(files, NULL);
    }

    void AppState::RunEditor(Panel& panel, String const& pathname)
    {
        HRESULT hr = Extern::Run(ExtrnAkelpad, panel.GetCurrentPath(), pathname);

        if (S_OK != hr)
            hr = Extern::Run(ExtrnNotepad, panel.GetCurrentPath(), pathname);

        SetMainframeStatus(Error((int)hr, std::system_category())
            , Fl::Entry(pathname, true).LoadShellIcon()
            , _LS(StrId_Launcheditorfors)
            , pathname.c_str()
            );
    }

    void AppState::EditEntry()
    {
        if (FItem* it = ActivePanel().GetHot())
            RunEditor(ActivePanel(), it->FileName);
    }

    void AppState::CreateEntry()
    {
        String pathname = ActivePanel().CreateEntry();
        if (!pathname.empty())
            RunEditor(ActivePanel(), pathname);
    }

    void AppState::Delete()                     
    {
// ##TODO: Implementation"))
    }

    void AppState::CopyRequest()                { PerformOperation(ActivePanel(), CopyFiles(GetInactivePanel().GetCurrentPath()), GetMainframeHwnd()); }
    void AppState::MoveRequest()                { PerformOperation(ActivePanel(), MoveFiles(GetInactivePanel().GetCurrentPath()), GetMainframeHwnd()); }
    void AppState::MoveToTrash()                { PerformOperation(ActivePanel(), DeleteToTrash(), GetMainframeHwnd()); }
    void AppState::CreateDirectoryRequest()     { ActivePanel().CreateDir(); }
    void AppState::RenameRequest()              { ActivePanel().View.StartLabelEdit(); }
    void AppState::ShowEntryProperties()        { ActivePanel().OnEntryProperties(); }
    void AppState::TrackHistoryPopup()          { ActivePanel().GetPathBar().TrackHistoryPopup(true); }
    void AppState::StartPathEdit()              { ActivePanel().StartPathEdit(); }
    void AppState::AddCurrentPathToBookmarks()  { ActivePanel().AddCurrentPathToBookmarks(); }
    void AppState::XorSelection()               { ActivePanel().View.XorSelection(true); }
    void AppState::AddSelection()               { ActivePanel().View.AddSubSelection(true, true); }
    void AppState::SubstractSelection()         { ActivePanel().View.AddSubSelection(false, true); }
    void AppState::EnterItem()                  { ActivePanel().View.EnterItem(); }
    void AppState::SelectInsertToggle()         { ActivePanel().View.ToogleSelectionAndMoveCursor(true); }
    void AppState::MoveCursorUp()               { ActivePanel().View.MoveCursorUpDown(false); }
    void AppState::MoveCursorDown()             { ActivePanel().View.MoveCursorUpDown(true); }
    void AppState::MoveCursorUpAndSelect()      { ActivePanel().View.ToogleSelectionAndMoveCursor(false); }
    void AppState::MoveCursorDownAndSelect()    { ActivePanel().View.ToogleSelectionAndMoveCursor(true); }
    void AppState::MoveCursorHome()             { ActivePanel().View.MoveCursorHomeEnd(false); }
    void AppState::MoveCursorEnd()              { ActivePanel().View.MoveCursorHomeEnd(true); }
    void AppState::MoveCursorPageUp()           { ActivePanel().View.MoveCursorPageUpDown(false); }
    void AppState::MoveCursorPageDown()         { ActivePanel().View.MoveCursorPageUpDown(true); }

    void AppState::UpdateCommandLinePath()
    {
        Commandline.SetPath(ActivePanel().GetCurrentPath());
    }

    bool AppState::OnEnterCommandLine(CString const& rawLine)
    {
        String commandLine = CT2W(rawLine).m_psz;
        Extern::Item it(L"FromCommandLine", commandLine);

        PCWSTR pwd = ActivePanel().GetCurrentPath();

        HRESULT hr = Twins::Extern::Run(it, pwd, L"");

// ##TODO: Configure `Run in terminal` if `ERROR_FILE_NOT_FOUND == hr`"))
        if (ERROR_FILE_NOT_FOUND == hr)
        {
            Extern::Item cmd(L"CMD", L"%COMSPEC%");

// ##TODO: Configure `/C` instead `/K`"))
            String args = L"/D /T:F8 /C \"" + commandLine + L"\"";
            hr = Twins::Extern::Run(cmd, pwd, args);
        }

        return S_OK == hr;
    }

    void AppState::OnCommandLineCancel(WTL::CComboBoxEx& combo) { ActivePanel().SetFocus(); }
    void AppState::ActivateCommandLine() { Commandline.SetFocus(); }
    void AppState::DropdownCommandLine() { Commandline.ShowDropdown(); }
    void AppState::PutPathToCommandLine() { Commandline.AddText(ActivePanel().GetCurrentPath(), true); }

    void AppState::PutHotNameToCommandLine() 
    {
        if (FItem* it = ActivePanel().View.GetHot())
            Commandline.AddText(it->FileName, false); 
    }

    void AppState::FindRequest()
    {
        FindDialog dialog(ActivePanel().GetCurrentPath(), FindDialogConfig, *this);
        INT_PTR rv = dialog.DoModal();

        if (FindDialog::ResultEntryesToPanel == rv)
        {
            ActivePanel().SetFiles(dialog.GetFiles(), _L(StrId_Searchresult));
        } 
        else if (FindDialog::ResultGotoEntry == rv)
        {
            Fl::Entry const& choosen = dialog.GetChoosen();
            if (choosen.IsValid())
                ActivePanel().GotoEntry(choosen);
        }
    }

    void AppState::CalcDirSizeRequest(bool multiple) 
    {
// ##TODO: Implementation"))
        _NotImplementedYet(_LS(StrId_Subdirectorysize)); 
    }

    void AppState::ExitRequest() 
    {
        // ##TODO: Destroy mainframe"))
        ::DestroyWindow(MainFrameRef); 
    }

    void AppState::AboutRequest()
    {
        AboutDialog aboutBox;
        aboutBox.DoModal(MainFrameRef);
    }

    void AppState::OnConfigurationCahnged(AppConfigDialog& configDlg)
    {
        if (!configDlg.IsLanguageChaged())
        {
            if (::IsWindow(MainFrameRef))
            {
                ::InvalidateRect(MainFrameRef, NULL, FALSE);
                LPanel->Reload();
                RPanel->Reload();
            }
        }
    }

    void AppState::ShowConfigDialog()
    {
        AppConfigDialog configBox(_LS(StrId_Configuration), *this, std::bind(&AppState::OnConfigurationCahnged, this, _1));

        INT_PTR dialogResult = configBox.DoModal();

        if (AppConfigDialog::ResultRestartRequired == dialogResult)
            RestartMainframeRequest();

        else if (AppConfigDialog::ResultImport == dialogResult)
            ShowConfigDialog();
    }

    void AppState::RestartMainframeRequest()
    {
        MainFrameRef.Restart();
    }
}
