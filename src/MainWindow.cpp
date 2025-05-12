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





 //UI control handling
        if (m_ui->freqSpinBox) {
        connect(m_ui->freqSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, [this](double value) {
                    if (m_gl) {
                        m_gl->updateTerrainFrequency(static_cast<float>(value));
                    }
                });
        }

        if (m_ui->octavesSpinBox) {
            connect(m_ui->octavesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                    this, [this](int value) {
                        if (m_gl) {
                            m_gl->updateTerrainOctaves(value);
                        }
                    });
        }

        if (m_ui->widthHorizontalSlider) {
            connect(m_ui->widthHorizontalSlider, &QSlider::valueChanged,
                    this, [this](int value) {
                        if (m_gl) {
                            m_gl->updateGridWidth(value);
                            if (m_ratioLocked && m_ui->depthVerticalSlider->value() != value) {
                                m_ui->depthVerticalSlider->setValue(value);
                            }
                        }
                    });
        }

        if (m_ui->depthVerticalSlider) {
            connect(m_ui->depthVerticalSlider, &QSlider::valueChanged,
                    this, [this](int value) {
                        if (m_gl) {
                            m_gl->updateGridDepth(value);
                            if (m_ratioLocked && m_ui->widthHorizontalSlider->value() != value) {
                                m_ui->widthHorizontalSlider->setValue(value);
                            }
                        }
                    });
        }


            if (m_ui->ratioLockCheckBox) {

                connect(m_ui->ratioLockCheckBox, &QCheckBox::stateChanged,
                        this, [this](int state) {
                            m_ratioLocked = (state == Qt::Checked);
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
