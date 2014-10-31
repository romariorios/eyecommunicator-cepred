#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <baseeyetrackerplugin.hpp>

#include <QDebug>
#include <QFileDialog>
#include <QPluginLoader>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto pluginFile = QFileDialog::getOpenFileName();
    QPluginLoader loader{pluginFile};

    auto dummy = qobject_cast<BaseEyetrackerPlugin *>(loader.instance());
    for (auto param : dummy->availableTrackingParams()) {
        qDebug() << param.fullName << " (" << param.name << ")";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
