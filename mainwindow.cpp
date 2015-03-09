#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include "tableview.h"
#include "memorygame.h"

#include <QFileDialog>
#include <QMessageBox>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    _eyetracker.addPluginPath(QFileDialog::getExistingDirectory());


    ui->setupUi(this);
    if(_eyetracker.isPluginSet())
        this->ui->menuRastreamento->setEnabled(true);

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
    connect(ui->actionCalibrarRastreamento, &QAction::triggered, [this]()
    {
        // TODO usar widget de rastreamento para plugins que têm
        _eyetracker.calibrate({});
    });
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
    this->selTable.addImg(imgListPath[index.row()],index.row());
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
    if(index == 3)
    {
        //this->ui->treeImages->setEnabled(false);
        //this->ui->listImages->setEnabled(false);
    }
    else
    {
        this->ui->treeImages->setEnabled(true);
        this->ui->listImages->setEnabled(true);
    }

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
    int N=this->ui->gridCol->value()*this->ui->gridLines->value();
    if(this->ui->type->currentIndex()==3 && N%2)         // if the gridsize is even in memory game
    {
        int ret = QMessageBox::information(this, tr("Erro na escolha da grade!"),
                                       tr("Só é possível escolher um número de cartas pares\n"
                                          "no jogo de memória."),
                                       QMessageBox::Ok);
        return;
    }
    this->selTable.setTimeSel(this->ui->timeSel->value());
    if(this->ui->type->currentIndex()==3)  // memory Game
    {
        memoryGame *mg;
        mg = new memoryGame(this);
        mg->setTable(this->selTable);
        mg->showFullScreen();
        connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [mg](const EyesPosition &e)
        {
            mg->setPt({ e.gaze.x() * mg->width(), e.gaze.y() * mg->height() });
        });
        connect(mg, SIGNAL(finished(int)), mg, SLOT(deleteLater()));


    }
    else
    {

        tableView *tb;
        tb=new tableView(this);
        tb->setTable(this->selTable);
        tb->showFullScreen();

        connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [tb](const EyesPosition &e)
        {
            tb->setPt({ e.gaze.x() * tb->width(), e.gaze.y() * tb->height() });
        });
        connect(tb, SIGNAL(finished(int)), tb, SLOT(deleteLater()));
    }
}

void MainWindow::on_random_clicked()
{

}
