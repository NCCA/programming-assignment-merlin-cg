#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "NGLScene.h"
#include "qlabel.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool m_ratioLocked = false;

private slots:

    void on_totalDropletsDial_valueChanged(int value);
    void on_erodeButton_clicked();

    void on_lifetimeDial_valueChanged(int value);

private:
    Ui::MainWindow *m_ui;
    NGLScene *m_gl;
   // QDial *durationDial;
    //QLCDNumber *durationLabel;
    int stepValue = 1000;
    int maxDroplets = 40000;
    int lifetime = 30;
};


#endif // MAINWINDOW_H
