#include "MainWindow.h"
#include <QFile>
#include <QTextStream>
#include <QApplication>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(6);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    QApplication a(argc, argv);

    // Load the stylesheet from Qt resource
    QFile f(":/qdarkstyle/dark/darkstyle.qss");
    if (!f.exists()) {
        qWarning("Unable to set stylesheet, file not found");
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

    MainWindow w;
    w.show();
    return a.exec();
}
