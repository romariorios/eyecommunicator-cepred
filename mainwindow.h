#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QStringList>
#include <QLabel>
#include "table.h"

#include "eyetracker.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QFileSystemModel *model;
    table selTable;
    QStringList imgListPath;

private slots:
    void on_treeImages_clicked(const QModelIndex &index);

    void on_listImages_clicked(const QModelIndex &index);

    void on_listSelected_clicked(const QModelIndex &index);

    void on_gridSize_currentIndexChanged(int index);

    void on_type_currentIndexChanged(int index);

    void on_gridLines_valueChanged(int arg1);

    void on_gridCol_valueChanged(int arg1);

    void on_run_clicked();

    void on_random_clicked();

    void on_actionAdcDiretorio_triggered();

    void on_actionSelPlugin_triggered();

private:
    bool tryStart(int pluginIndex, const QVariantHash &params = {});
    void setPluginState(bool isStarted, int pluginIndex);

    QLabel _statusBarWidget;
    Ui::MainWindow *ui;
    Eyetracker _eyetracker;
    void changeTablePar();
    QStringList allImagesPath;  // all images path for memory game purpose
    void loadImagesDir(QString pth, QStringList *listImg);
};

#endif // MAINWINDOW_H
