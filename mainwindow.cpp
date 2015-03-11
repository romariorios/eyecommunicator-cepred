#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include "tableview.h"
#include "memorygame.h"

#include <QFileDialog>
#include <QMessageBox>
#include <algorithm>
#include <QDesktopWidget>
#include <QSettings>
#include <QVBoxLayout>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
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

    show();

    auto &&pluginPaths = QSettings{}.value("pluginPaths", QStringList{}).toStringList();
    for (auto &&path : pluginPaths) {
        _eyetracker.addPluginPath(path);
    }

    auto selectedPlugin = QSettings{}.value("selectedPlugin", -1).toInt();
    _eyetracker.setCurrentPlugin(selectedPlugin);
    if (!_eyetracker.start())
        on_actionSelPlugin_triggered();
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
    QDesktopWidget *widget = QApplication::desktop();
    QRect rect = widget->screenGeometry(0);             // get actual size of window

    this->selTable.setTimeSel(this->ui->timeSel->value());
    if(this->ui->type->currentIndex()==3)  // memory Game
    {
        memoryGame *mg;
        mg = new memoryGame(this);
        mg->setTable(this->selTable);
//        mg->show();
//        mg->move(rect.width()+10, rect.y());

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
        tb->move(rect.width()+10, rect.y());
        tb->showFullScreen();

        connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [tb](const EyesPosition &e)
        {
            tb->setPt({ e.gaze.x() * tb->width(), e.gaze.y() * tb->height() });
        });
        connect(tb, SIGNAL(finished(int)), tb, SLOT(deleteLater()));
    }
}
void MainWindow::loadImagesDir(QString pth,QStringList *listImg)
{
    QDir dir(pth);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i){
        QFileInfo fileInfo = list.at(i);
        listImg->push_back(fileInfo.filePath());
    }
    dir.setFilter(QDir::AllDirs|QDir::NoDotDot|QDir::NoDot);
    list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i){
        QFileInfo fileInfo = list.at(i);
        this->loadImagesDir(fileInfo.filePath(),listImg);
    }

}

void MainWindow::on_random_clicked()
{
    class seleItem{
    public:
        QString path;
        int id;
    };
    vector <seleItem> selectedPath;
    QString pth=QDir::currentPath()+"/Images";
    if(this->allImagesPath.isEmpty()){
        this->loadImagesDir(pth,&this->allImagesPath);
    }

    random_shuffle(allImagesPath.begin(),allImagesPath.end());
    int N=this->ui->gridCol->value()*this->ui->gridLines->value();
    if(this->ui->type->currentIndex()==3){   // Memory Game
        for(int i=0;i<N/2;i++){
            seleItem it;
            it.path=this->allImagesPath[i];
            it.id=i;
            selectedPath.push_back(it);
            selectedPath.push_back(it);
        }
    }
    else{
        for(int i=0;i<N;i++){
            seleItem it;
            it.path=this->allImagesPath[i];
            it.id=i;
            selectedPath.push_back(it);
        }
    }


    if(N>selectedPath.size())
        return;

    random_shuffle(selectedPath.begin(),selectedPath.end());
    this->ui->listSelected->clear();
    this->selTable.clearAll();

    for (int i = 0; i < selectedPath.size(); ++i){
        QFileInfo fileInfo;
        fileInfo.setFile(selectedPath[i].path);
        this->ui->listSelected->addItem(fileInfo.baseName());
        this->ui->listSelected->item(i)->setIcon(QIcon(selectedPath[i].path));
        this->selTable.addImg(selectedPath[i].path,selectedPath[i].id);
        this->imgListPath.push_back(selectedPath[i].path);
    }
}

void MainWindow::on_actionAdcDiretorio_triggered()
{
    auto dir = QFileDialog::getExistingDirectory();
    if (dir.isEmpty())
        return;

    QSettings s;
    auto &&pluginPaths = s.value("pluginPaths", QStringList{}).toStringList();
    pluginPaths << dir;
    s.setValue("pluginPaths", pluginPaths);

    _eyetracker.addPluginPath(dir);
}

void MainWindow::on_actionSelPlugin_triggered()
{
    QDialog d;
    QVBoxLayout l{&d};
    QListWidget plugins;
    QPushButton select{"Selecionar"};

    d.setWindowTitle("Selecionar plugin");
    l.addWidget(&plugins);
    l.addWidget(&select);

    for (auto &&plugin : _eyetracker.pluginsFound())
        plugins.addItem(plugin);

    connect(&select, &QPushButton::clicked, &d, &QDialog::close);
    connect(&select, &QPushButton::clicked, [&]()
    {
        auto row = plugins.currentIndex().row();
        _eyetracker.setCurrentPlugin(row);
        if (_eyetracker.start())
            QSettings{}.setValue("selectedPlugin", row);
    });

    d.exec();
}
