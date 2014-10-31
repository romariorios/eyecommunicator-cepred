#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <baseeyetrackerplugin.hpp>

#include <QDebug>
#include <QPluginLoader>

Q_IMPORT_PLUGIN(dummyeyetrackerplugin)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << QPluginLoader::staticInstances();
}

MainWindow::~MainWindow()
{
    delete ui;
}
