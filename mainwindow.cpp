#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <baseeyetrackerplugin.hpp>

#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _eyetracker.addPluginPath(QFileDialog::getExistingDirectory());

    connect(
        ui->actionStart_tracking, SIGNAL(triggered()),
        &_eyetracker, SLOT(start()));

    connect(ui->actionCalibrate, &QAction::triggered, [this]()
    {
        // TODO use calibration widget for engines that require it
        _eyetracker.calibrate({});
    });

    connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [](const EyesPosition &pos)
    {
        qDebug() << "Gaze:" << pos.gaze << "Left:" << pos.leftEye << "Right:" << pos.rightEye;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
