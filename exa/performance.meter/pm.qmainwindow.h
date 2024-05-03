#pragma once

#include <memory>
#include <QtCore/QObject>
#include <QtGui/QMainWindow>

class Ui_MainWindow;

namespace Pm
{
    class MainFrame: QMainWindow
    {
    private:
        typedef QMainWindow Super;
        Q_OBJECT
        Q_DISABLE_COPY(MainFrame)

    public:
        MainFrame(QWidget* parent = NULL, Qt::WindowFlags flags = Qt::Window);
        virtual ~MainFrame();

        using Super::show;

    public Q_SLOTS:
        void onAppAbout();

    private:
        std::auto_ptr<Ui_MainWindow> Layout;
    };
}
