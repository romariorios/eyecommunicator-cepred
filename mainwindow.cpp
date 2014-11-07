#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <baseeyetrackerplugin.hpp>

#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>

#include "pluginconfigdialog.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _eyetracker.addPluginPath(QFileDialog::getExistingDirectory());

    connect(ui->actionStart_tracking, &QAction::triggered, [=]()
    {
        PluginConfigDialog d{_eyetracker.params()};

        if (d.exec()) {
            _eyetracker.start(d.result());
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
