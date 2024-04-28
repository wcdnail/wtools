#pragma once

#include "panel.h"
#include "dh.tracing.h"
#include "extern.item.h"
#include "command.line.h"
#include "twins.main.frame.h"
#include "windows.gui.leaks.h"
#include <shell.imagelist.h>
#include <settings.h>
#include <boost/noncopyable.hpp>

namespace Twins
{
    struct Application;

    Application& App();

    struct Application: boost::noncopyable
    {
        using ScopedTLPtr = std::unique_ptr<Dh::ScopedThreadLog>;
        using GuiLeaksPtr = std::unique_ptr<CF::GUILeaks>;

        ScopedTLPtr          LiveTimer;
        GuiLeaksPtr            UILeaks;
        CAppModule           AtlModule;
        unsigned            LanguageId;
        CIcon                     Icon;
        bool      IsMainframeMaximized;
        CRect                  AppRect;
        CRect               ViewerRect;
        bool                ShowHidden;
        int           ActivePanelIndex;
        Extern::Item     ExtrnTerminal;
        Extern::Item      ExtrnAkelpad;
        Extern::Item      ExtrnNotepad;
        Sh::Imagelist       ShellIcons;
        Sh::Imagelist  ShellSmallIcons;
        CMessageLoop       MessageLoop;
        MainFrame             AppFrame;
        Ui::CommandLine    Commandline;
        int              ActivePanelId;
        Panel                   Panels[2];
        int                ButtonBarId[8];

        HRESULT Run(HINSTANCE baseAddress, int showCmd);
        bool RunSome(CString const& rawLine);
        void SetLocale(unsigned id);
        void Exit();

    private:
        friend Application& App();

        Application();
        ~Application();
                
        Conf::Section Options;
    };
}
