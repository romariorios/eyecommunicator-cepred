#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include "tableview.h"
#include "rndtableview.h"
#include "memorygame.h"

#include <QFileDialog>
#include <QMessageBox>
#include <algorithm>
#include <QDesktopWidget>
#include <QSettings>
#include <QVBoxLayout>
#include <time.h>
#include <iostream>
#include <fstream>
#include <QProgressDialog>
#include <QCoreApplication>

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
    this->loadTemplates();
    model->sort(0);

    srand(time(NULL));

    statusBar()->addWidget(&_statusBarWidget);

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

    QSettings s;
    auto selectedPlugin = s.value("selectedPlugin", -1).toInt();
    auto &&pluginParams = s.value("pluginParams", QVariantHash{}).toHash();
    if (!tryStart(selectedPlugin, pluginParams))
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
    auto &&list = dir.entryInfoList();
    int tam=list.size();
    QProgressDialog progress(tr("Carregando imagens..."), tr("Cancela"), 0, tam, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    for (int i = 0; i < tam; ++i){
        const auto &fileInfo = list.at(i);
        this->ui->listImages->addItem(fileInfo.baseName());
        this->ui->listImages->item(i)->setIcon(QIcon(fileInfo.filePath()));
        this->imgListPath.push_back(QDir().relativeFilePath(fileInfo.filePath()));
        progress.setValue(i);
        QCoreApplication::processEvents();
        if(progress.wasCanceled())
            break;
    }
    progress.close();
}

void MainWindow::on_listImages_clicked(const QModelIndex &index)
{
    int N = this->ui->gridLines->value() * this->ui->gridCol->value();
    if(this->ui->listSelected->count() >= N && this->ui->type->currentIndex()!=1)
        return;
    auto it= new QListWidgetItem(ui->listSelected);
    *it=*this->ui->listImages->item(index.row());
    it->setFlags(it->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    it->setCheckState(Qt::Checked); // AND initialize check state
    this->ui->listSelected->addItem(it);
    this->selTable.addImg(imgListPath[index.row()],index.row());
}

void MainWindow::on_listSelected_clicked(const QModelIndex &index)
{
    if(this->selTable.getCell(index.row()).eyeSelectable)
    {
        this->selTable.setCellEyeSelectable(index.row(),false);
        this->ui->listSelected->item(index.row())->setCheckState(Qt::Unchecked);
    }
    else
    {
        this->selTable.setCellEyeSelectable(index.row(),true);
        this->ui->listSelected->item(index.row())->setCheckState(Qt::Checked);
    }
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
    if(index==3) // grid lateral
    {
        this->ui->gridSize->setCurrentIndex(0); // 1x2
        this->ui->gridSize->setEnabled(false);
        this->ui->gridCol->setValue(2);
        this->ui->gridLines->setValue(1);
        this->ui->gridCol->setEnabled(false);
        this->ui->gridLines->setEnabled(false);
    }
    else
    {
        this->ui->gridSize->setEnabled(true);
        this->ui->gridCol->setEnabled(true);
        this->ui->gridLines->setEnabled(true);
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
    if(this->ui->type->currentIndex()==2 && N%2)         // if the gridsize is even in memory game
    {
        QMessageBox::information(
                    this,
                    tr("Erro na escolha da grade!"),
                    tr("Só é possível escolher um número de cartas pares\n"
                       "no jogo de memória."),
                    QMessageBox::Ok);
        return;
    }

    this->selTable.setTimeSel(this->ui->timeSel->value());
    this->selTable.setText(this->ui->tableText->text());

    switch (this->ui->type->currentIndex())
    {
    case 0:  // Normal Grid
        tableView *tb;
        tb=new tableView(this);
        tb->setTable(this->selTable);
        tb->setIsLateral(false);

        connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [tb](const EyesPosition &e)
        {
            tb->setPt({ e.gaze.x() * tb->width(), e.gaze.y() * tb->height() });
        });
        connect(tb, SIGNAL(finished(int)), tb, SLOT(deleteLater()));
        tb->exec();
        break;
    case 1:  // Random Grid
        rndTableView *rtb;
        rtb=new rndTableView(this);
        rtb->setTable(this->selTable);

        connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [rtb](const EyesPosition &e)
        {
            rtb->setPt({ e.gaze.x() * rtb->width(), e.gaze.y() * rtb->height() });
        });
        connect(rtb, SIGNAL(finished(int)), rtb, SLOT(deleteLater()));
        rtb->exec();
        break;
    case 2:  // memory Game
        memoryGame *mg;
        mg = new memoryGame(this);
        mg->setTable(this->selTable);
        connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [mg](const EyesPosition &e)
        {
            mg->setPt({ e.gaze.x() * mg->width(), e.gaze.y() * mg->height() });
        });
        connect(mg, SIGNAL(finished(int)), mg, SLOT(deleteLater()));
        mg->exec();
        break;
    case 3:  // double Lateral Grid
        tableView *ltb;
        ltb=new tableView(this);
        ltb->setTable(this->selTable);
        ltb->setIsLateral(true);

        connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [ltb](const EyesPosition &e)
        {
            ltb->setPt({ e.gaze.x() * ltb->width(), e.gaze.y() * ltb->height() });
        });
        connect(ltb, SIGNAL(finished(int)), ltb, SLOT(deleteLater()));
        ltb->exec();
        break;
    }
}
void MainWindow::loadImagesDir(QString pth,QStringList *listImg)
{
    QDir dir(pth);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    auto list = dir.entryInfoList();
    for (auto fileInfo : list) {
        listImg->push_back(QDir().relativeFilePath(fileInfo.filePath()));
    }
    dir.setFilter(QDir::AllDirs|QDir::NoDotDot|QDir::NoDot);
    list = dir.entryInfoList();
    for (auto fileInfo : list){
        this->loadImagesDir(fileInfo.filePath(),listImg);
    }

}

void MainWindow::loadTemplates()
{
    QDir dir(QDir::currentPath()+"/Pranchas");
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QStringList filters;
    filters << "*.tbl" ;
    dir.setNameFilters(filters);
    auto list = dir.entryInfoList();
    for (auto fileInfo : list) {
        this->templateListPath.push_back(fileInfo.filePath());
        this->ui->templates->addItem(fileInfo.baseName());
    }
}

void MainWindow::on_random_clicked()
{
    int N=this->ui->gridCol->value()*this->ui->gridLines->value();
    if(this->ui->type->currentIndex()==2 && N%2)         // if the gridsize is even in memory game
    {
        int ret = QMessageBox::information(this, tr("Erro na escolha da grade!"),
                                           tr("Só é possível escolher um número de cartas pares\n"
                                              "no jogo de memória."),
                                           QMessageBox::Ok);
        return;
    }

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
    if(this->ui->type->currentIndex()==2){   // Memory Game
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
    this->imgListPath.clear();
    this->ui->listImages->clear();

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
        if (!tryStart(plugins.currentIndex().row()))
            QMessageBox::critical(
                        this,
                        "Plugin não carregado",
                        "Houve um problema ao carregar o plugin");
    });

    d.exec();
}

bool MainWindow::tryStart(int pluginIndex, const QVariantHash &params)
{
    _eyetracker.setCurrentPlugin(pluginIndex);
    auto started = params.empty()? _eyetracker.start() : _eyetracker.start(params);

    QSettings s;
    if (started) {
        s.setValue("selectedPlugin", pluginIndex);
        s.setValue("pluginParams", _eyetracker.currentParams());
    } else {
        s.remove("selectedPlugin");
        s.remove("pluginParams");
    }

    setPluginState(started, pluginIndex);
    return started;
}

void MainWindow::setPluginState(bool isStarted, int pluginIndex)
{
    _statusBarWidget.setText(
                isStarted?
                    "Plugin carregado: " + _eyetracker.pluginsFound()[pluginIndex] :
                    "NENHUM PLUGIN CARREGADO");
    ui->run->setEnabled(isStarted);
}

void MainWindow::openTemplate(QString nome)
{
    ifstream ifp;
    ifp.open(nome.toLocal8Bit());
    int type,lines,cols;
    double time;
    string text;
    int size=0;
    // Fake loading only for get Size
    ifp >> type;
    ifp >> cols;
    ifp >> lines;
    ifp >> time;
    ifp.ignore();
    getline(ifp,text);
    QString Qpath;
    do
    {
        string pat;
        int id;
        bool eyeSel;
        getline(ifp,pat);
        ifp >> id;
        ifp >> eyeSel;
        ifp.ignore();
        Qpath= QString::fromLocal8Bit(pat.c_str());
        size++;
    }while(!Qpath.isEmpty());
    ifp.close();

    // Real loading
    // load grid information


    ifp.open(nome.toLocal8Bit());

    ifp >> type;
    this->ui->type->setCurrentIndex(type);
    ifp >> cols;
    this->ui->gridCol->setValue(cols);
    ifp >> lines;
    this->ui->gridLines->setValue(lines);
    ifp >> time;
    this->ui->timeSel->setValue(time);
    ifp.ignore();
    getline(ifp,text);
    this->ui->tableText->setText(QString().fromLatin1(text.c_str()));

    // load images
    QProgressDialog progress(tr("Carregando prancha..."), tr("Cancela"), 0, size, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    this->selTable.clearAll();
    this->ui->listSelected->clear();
    int i=0;
    do
    {
        string pat;
        int id;
        bool eyeSel;
        getline(ifp,pat);
        ifp >> id;
        ifp >> eyeSel;
        ifp.ignore();
        Qpath= QString::fromLocal8Bit(pat.c_str());
        if(!Qpath.isEmpty())
        {
            QFileInfo fileInfo;
            fileInfo.setFile(Qpath);
            this->ui->listSelected->addItem(fileInfo.baseName());
            int i=this->ui->listSelected->count()-1;
            this->ui->listSelected->item(i)->setIcon(QIcon(Qpath));
            this->ui->listSelected->item(i)->setFlags(this->ui->listSelected->item(i)->flags() | Qt::ItemIsUserCheckable);

            this->selTable.addImg(Qpath,id);
            this->selTable.setCellEyeSelectable(this->selTable.getListCellSize()-1,eyeSel);
            if(eyeSel)
                this->ui->listSelected->item(i)->setCheckState(Qt::Checked);
            else
                this->ui->listSelected->item(i)->setCheckState(Qt::Unchecked);
            progress.setValue(++i);
            QCoreApplication::processEvents();
            if(progress.wasCanceled())
                break;

        }
    }while(!Qpath.isEmpty());
    progress.close();
    ifp.close();
    this->repaint();
}

void MainWindow::on_actionSalvar_triggered()
{
    QString nome=QFileDialog::getSaveFileName(this,"Select output file to save",
                                              "Pranchas", "Tabelas (*.tbl)");
    if(nome.isEmpty())
        return;
    ofstream ofp;
    ofp.open(nome.toLocal8Bit());
    // save grid information
    ofp << this->ui->type->currentIndex()<<endl;
    ofp << this->ui->gridCol->value()<<endl;
    ofp << this->ui->gridLines->value() << endl;
    ofp << this->ui->timeSel->value() << endl;
    ofp << this->ui->tableText->text().toStdString()<<endl;
    // save images paths
    for(int i=0;i<this->selTable.count();i++)
    {
        ofp << this->selTable.getImgPath(i).toLocal8Bit().toStdString() <<endl;
        ofp << this->selTable.getId(i)<<endl;
        ofp << this->selTable.getCell(i).eyeSelectable<<endl;
    }

    ofp.close();
    this->templateListPath.clear();
    this->ui->templates->clear();
    this->loadTemplates();
}




void MainWindow::on_actionAbrir_triggered()
{
    QString nome=QFileDialog::getOpenFileName(this,"Select file to open",
                                              "", "Tabelas (*.tbl)");
    if(nome.isEmpty())
        return;
    this->openTemplate(nome);
}



void MainWindow::on_templates_doubleClicked(const QModelIndex &index)
{
    int i=this->ui->templates->currentRow();
    int j= index.row();
    this->openTemplate(this->templateListPath[i]);
}

void MainWindow::on_clearTable_clicked()
{
    this->ui->listSelected->clear();
    this->selTable.clearAll();
    this->ui->tableText->clear();
    this->repaint();
}

void MainWindow::on_listSelected_doubleClicked(const QModelIndex &index)
{
    this->selTable.delImg(index.row());
    this->ui->listSelected->takeItem(index.row());

}
