#include "../include/MainWindow.h"
#include "ui/ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    m_gl = new NGLScene(this);
    m_ui->m_MainWindowgridLayout->addWidget(m_gl,0,0,2,1);
    connect(m_ui->m_spread,SIGNAL(valueChanged(double)),
            m_gl,SLOT(setSpread(double)));

    connect(m_gl,&NGLScene::glInitialized,[=](){

    // auto emitter=m_gl->getEmitter();
    //     connect(m_ui->m_numPerFrame,SIGNAL(valueChanged(int)),
    //         emitter,SLOT(setNumPerFrame(int)));
    });
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
