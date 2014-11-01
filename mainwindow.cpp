#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <baseeyetrackerplugin.hpp>

#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _eyetracker.addPluginPath(QFileDialog::getExistingDirectory());

    connect(ui->actionStart_tracking, &QAction::triggered, [=]()
    {
        QDialog d{this};
        QVBoxLayout l{&d};

        d.setWindowTitle(tr("Eyetracker parameters"));

        for (auto param : _eyetracker.params()) {
            auto fieldLayout = new QHBoxLayout;
            l.addLayout(fieldLayout);

            fieldLayout->addWidget(new QLabel{param.fullName, &d});
            switch (param.type) {
            case BaseEyetrackerPlugin::Param::Int:
            {
                auto spinBox = new QSpinBox{&d};
                spinBox->setRange(param.values[0].toInt(), param.values[1].toInt());
                fieldLayout->addWidget(spinBox);
                break;
            }
            case BaseEyetrackerPlugin::Param::Float:
            {
                auto spinBox = new QDoubleSpinBox{&d};
                spinBox->setRange(param.values[0].toInt(), param.values[1].toInt());
                fieldLayout->addWidget(spinBox);
                break;
            }
            case BaseEyetrackerPlugin::Param::String:
                fieldLayout->addWidget(new QLineEdit{&d});
                break;
            case BaseEyetrackerPlugin::Param::Set:
            {
                auto comboBox = new QComboBox{&d};
                for (auto value : param.values) {
                    comboBox->addItem(value.toString());
                }

                fieldLayout->addWidget(comboBox);
                break;
            }
            default:
                fieldLayout->addWidget(new QLabel{tr("(invalid)"), &d});
            }
        }

        QPushButton okButton{tr("Start")};
        l.addWidget(&okButton);
        connect(&okButton, &QPushButton::clicked, &d, &QDialog::accept);

        if (d.exec()) {
            _eyetracker.start({{ "period", 300 }});
        }
    });

    connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [=](const EyesPosition &pos)
    {
        qDebug() << "Gaze:" << pos.gaze << "Left:" << pos.leftEye << "Right:" << pos.rightEye;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
