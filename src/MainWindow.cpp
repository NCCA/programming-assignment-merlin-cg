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

        // Frequency
        connect(m_ui->freqSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, [this](double value) {
                        m_gl->updateTerrainFrequency(static_cast<float>(value));
                });

        // Octaves
            connect(m_ui->octavesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                    this, [this](int value) {
                            m_gl->updateTerrainOctaves(value);
                    });

        // Height
            connect(m_ui->heightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                    this, [this](int value) {
                            m_gl->updateTerrainHeight(value);
                    });


        // Depth
            connect(m_ui->widthHorizontalSlider, &QSlider::valueChanged,
                    this, [this](int value) {
                            m_gl->updateGridWidth(value);
                            if (m_ratioLocked && m_ui->depthVerticalSlider->value() != value) {
                                m_ui->depthVerticalSlider->setValue(value);
                            }
                    });

        // Width
            connect(m_ui->depthVerticalSlider, &QSlider::valueChanged,
                    this, [this](int value) {
                            m_gl->updateGridDepth(value);
                            if (m_ratioLocked && m_ui->widthHorizontalSlider->value() != value) {
                                m_ui->widthHorizontalSlider->setValue(value);
                            }
                    });

        // Width height lock
                connect(m_ui->ratioLockCheckBox, &QCheckBox::stateChanged,
                        this, [this](int state) {
                            m_ratioLocked = (state == Qt::Checked);
                        });

    });
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::on_totalDropletsDial_valueChanged(int value)
{
    int normalisedValue = value/stepValue;
    value = (std::round(normalisedValue)) * stepValue;

    m_ui->totalDropletsLabel->display(value);
    maxDroplets = value;

}
void MainWindow::on_lifetimeDial_valueChanged(int value)
{
    m_ui->lifetimeLabel->display(value);
    lifetime = value;
}



void MainWindow::on_erodeButton_clicked()
{
    m_gl->callErosionEvent(maxDroplets,lifetime);
}


