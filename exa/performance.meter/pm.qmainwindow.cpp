#include "precompiled.h"
#include "pm.qmainwindow.h"
#include "__gen/mainwindow.ui.h"
#include "__gen/aboutdialog.ui.h"

namespace Pm
{
    namespace Dialogs
    {
        class About: public QDialog
        {
        public:
            About() 
                : QDialog()
                , Logo(":/pixmaps/art/my.wife/_DSC3580.jpg")
                , Layout()
            {
                Layout.setupUi(this);
                //Layout.logo->(Logo);
            }

            ~About() 
            {}

        private:
            QPixmap Logo;
            Ui_AboutDialog Layout;
        };
    }

    MainFrame::MainFrame(QWidget* parent /* = NULL */, Qt::WindowFlags flags /* = Qt::Window */)
        : Super(parent, flags)
        , Layout(new Ui_MainWindow())
    {
        Layout->setupUi(this);
        statusBar()->showMessage(tr("Ready"), 3000);

        onAppAbout();
    }

    MainFrame::~MainFrame()
    {}

    void MainFrame::onAppAbout()
    {
        Dialogs::About dlg;
        dlg.exec();
    }
}
