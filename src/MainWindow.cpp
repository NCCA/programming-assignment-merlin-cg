#include "../include/MainWindow.h"
#include "ui/ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    m_gl = new NGLScene(this);
    m_ui->m_MainWindowgridLayout->addWidget(m_gl,0,0,2,1);
    connect(m_ui->freqSpinBox,SIGNAL(valueChanged(double)),
            m_gl,SLOT(setSpread(double)));

    connect(m_gl,&NGLScene::glInitialized,[=](){






        connect(m_ui->freqSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, [this](double value) {
                    if (m_gl) { // Check if m_gl is valid
                        m_gl->updateTerrainFrequency(static_cast<float>(value));
                    }
                });

        if (m_ui->octavesSpinBox) { // Check if the octaves spin box exists in your UI
            connect(m_ui->octavesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                    this, [this](int value) {
                        if (m_gl) { // Check if m_gl is valid
                            m_gl->updateTerrainOctaves(value);
                        }
                    });
        }



    //OLD EXAMPLE
    // auto emitter=m_gl->getEmitter();
    //     connect(m_ui->m_numPerFrame,SIGNAL(valueChanged(int)),
    //         emitter,SLOT(setNumPerFrame(int)));
    });
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
