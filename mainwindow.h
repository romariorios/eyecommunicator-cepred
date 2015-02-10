#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
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

private:
    Ui::MainWindow *ui;
    Eyetracker _eyetracker;
    void changeTablePar();
};

#endif // MAINWINDOW_H
