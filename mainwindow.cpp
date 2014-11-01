#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <baseeyetrackerplugin.hpp>

#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _eyetracker.addPluginPath(QFileDialog::getExistingDirectory());
    for (auto p : _eyetracker.pluginsFound()) {
        qDebug() << p;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
