#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include "tableview.h"

#include <QFileDialog>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    _eyetracker.addPluginPath(QFileDialog::getExistingDirectory());

    ui->setupUi(this);
    model = new QFileSystemModel;
    model->setFilter(QDir::AllDirs|QDir::NoDotDot|QDir::NoDot);
    model->setRootPath("");
    this->ui->treeImages->setModel(model);
    this->ui->treeImages->setAnimated(true);
    this->ui->treeImages->setIndentation(20);
    this->ui->treeImages->setSortingEnabled(true);
    QString pth=QDir::currentPath()+"/Images";
    this->ui->treeImages->setRootIndex(model->index(pth));
    this->ui->treeImages->hideColumn(2);
    this->ui->treeImages->hideColumn(3);
    this->ui->treeImages->hideColumn(4);
    this->ui->treeImages->hideColumn(1);
    this->ui->treeImages->resizeColumnToContents(0);
    this->ui->treeImages->expandAll();
    this->ui->listImages->setDragEnabled(false);
    this->ui->listSelected->setDragEnabled(false);
    this->selTable.setGridType(0); // type Grid
    this->selTable.setGridSize(QSize(2,1));

    connect(
        ui->actionIniciarRastreamento, SIGNAL(triggered()),
        &_eyetracker, SLOT(start()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_treeImages_clicked(const QModelIndex &index)
{
    QDir dir(model->filePath(index));
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);

    this->ui->listImages->clear();
    this->imgListPath.clear();
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i){
        QFileInfo fileInfo = list.at(i);
        this->ui->listImages->addItem(fileInfo.baseName());
        this->ui->listImages->item(i)->setIcon(QIcon(fileInfo.filePath()));
        this->imgListPath.push_back(fileInfo.filePath());
    }
}

void MainWindow::on_listImages_clicked(const QModelIndex &index)
{
    int N=this->ui->gridLines->value()*this->ui->gridCol->value();
    if(this->ui->listSelected->count()>=N)
        return;
    QListWidgetItem *it= new QListWidgetItem();
    *it=*this->ui->listImages->item(index.row());
    this->ui->listSelected->addItem(it);
    this->selTable.addImg(imgListPath[index.row()]);
}

void MainWindow::on_listSelected_clicked(const QModelIndex &index)
{
    this->selTable.delImg(index.row());
    QListWidgetItem *it=this->ui->listSelected->takeItem(index.row());
}

void MainWindow::on_gridSize_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        this->ui->gridLines->setValue(1);
        this->ui->gridCol->setValue(2);
        break;
    case 1:
        this->ui->gridLines->setValue(2);
        this->ui->gridCol->setValue(2);
        break;
    case 2:
        this->ui->gridLines->setValue(1);
        this->ui->gridCol->setValue(3);
        break;
    case 3:
        this->ui->gridLines->setValue(3);
        this->ui->gridCol->setValue(3);
        break;
    case 4:
        this->ui->gridLines->setValue(1);
        this->ui->gridCol->setValue(4);
        break;
    }

}

void MainWindow::on_type_currentIndexChanged(int index)
{
 changeTablePar();
}

void MainWindow::on_gridLines_valueChanged(int arg1)
{
 changeTablePar();
}

void MainWindow::on_gridCol_valueChanged(int arg1)
{
    changeTablePar();
}

void MainWindow::changeTablePar()
{
    this->selTable.setGridType(this->ui->type->currentIndex()); // type Grid
    this->selTable.setGridSize(QSize(this->ui->gridCol->value(),this->ui->gridLines->value()));
}

void MainWindow::on_run_clicked()
{
 tableView *tb;
 this->selTable.setTimeSel(this->ui->timeSel->value());
 tb=new tableView(this);
 tb->setTable(this->selTable);
 tb->showFullScreen();

 connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [tb](const EyesPosition &e)
 {
     tb->setPt({ e.gaze.x() * tb->width(), e.gaze.y() * tb->height() });
 });
 connect(tb, SIGNAL(finished(int)), tb, SLOT(deleteLater()));
}
