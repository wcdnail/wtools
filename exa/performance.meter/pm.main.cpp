#include "precompiled.h"
#include "pm.qmainwindow.h"

int main(int argc, char* argv[], char* envp[])
{
    QApplication app(argc, argv);

    Pm::MainFrame mainframe;
    mainframe.show();

    return app.exec();
}

#if _UNDER_WINDOWS && defined(_WINDOWS)
int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    extern bool usingWinMain;
    usingWinMain = true;
    qInstallMsgHandler(NULL);

    return main(__argc, __argv, _environ);
}
#endif // _UNDER_WINDOWS && defined(_WINDOWS)
