#include "mainwindow.h"
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
#include <QCollator>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if(_eyetracker.isPluginSet())
        ui->menuRastreamento->setEnabled(true);

    model = new QFileSystemModel;
    model->setFilter(QDir::AllDirs|QDir::NoDotDot|QDir::NoDot);
    model->setRootPath("");

    ui->treeImages->setModel(model);
    ui->treeImages->setAnimated(true);
    ui->treeImages->setIndentation(20);

    QString pth = QDir::current().absoluteFilePath("Images");
    ui->treeImages->setRootIndex(model->index(pth));

    ui->treeImages->hideColumn(2);
    ui->treeImages->hideColumn(3);
    ui->treeImages->hideColumn(4);
    ui->treeImages->hideColumn(1);

    ui->treeImages->resizeColumnToContents(0);
    ui->treeImages->expandAll();
    ui->listImages->setDragEnabled(false);
    ui->listSelected->setDragEnabled(false);

    selTable.setGridType(0); // type Grid
    selTable.setGridSize(QSize(2, 1));

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
    dir.setNameFilters({"*.svg", "*.png"});
    dir.setSorting(QDir::Name);
    _curDir = dir;

    loadTemplates(dir);

    ui->listImages->clear();
    imgListPath.clear();
    auto &&list = dir.entryInfoList();
    int tam = list.size();
    QProgressDialog progress(tr("Carregando imagens..."), tr("Cancela"), 0, tam, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    for (int i = 0; i < tam; ++i){
        const auto &fileInfo = list.at(i);

        ui->listImages->addItem(fileInfo.baseName());
        ui->listImages->item(i)->setIcon(QIcon(fileInfo.filePath()));
        imgListPath.push_back(QDir().relativeFilePath(fileInfo.filePath()));
        progress.setValue(i);

        QCoreApplication::processEvents();

        if (progress.wasCanceled())
            break;
    }
    progress.close();
}

void MainWindow::on_listImages_clicked(const QModelIndex &index)
{
    selectImage(index.row());
}

void MainWindow::on_listSelected_clicked(const QModelIndex &index)
{
    if (selTable.getCell(index.row()).eyeSelectable)
    {
        selTable.setCellEyeSelectable(index.row(), false);
        ui->listSelected->item(index.row())->setCheckState(Qt::Unchecked);
    }
    else
    {
        selTable.setCellEyeSelectable(index.row(), true);
        ui->listSelected->item(index.row())->setCheckState(Qt::Checked);
    }
}

void MainWindow::on_gridSize_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        ui->gridLines->setValue(1);
        ui->gridCol->setValue(2);
        break;
    case 1:
        ui->gridLines->setValue(2);
        ui->gridCol->setValue(2);
        break;
    case 2:
        ui->gridLines->setValue(1);
        ui->gridCol->setValue(3);
        break;
    case 3:
        ui->gridLines->setValue(3);
        ui->gridCol->setValue(3);
        break;
    case 4:
        ui->gridLines->setValue(1);
        ui->gridCol->setValue(4);
        break;
    }

}

void MainWindow::on_type_currentIndexChanged(int index)
{
    if (index == 3) // grid lateral
    {
        ui->gridSize->setCurrentIndex(0); // 1x2
        ui->gridSize->setEnabled(false);
        ui->gridCol->setValue(2);
        ui->gridLines->setValue(1);
        ui->gridCol->setEnabled(false);
        ui->gridLines->setEnabled(false);
    }
    else
    {
        ui->gridSize->setEnabled(true);
        ui->gridCol->setEnabled(true);
        ui->gridLines->setEnabled(true);
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
    selTable.setGridType(ui->type->currentIndex()); // type Grid
    selTable.setGridSize(QSize(ui->gridCol->value(),ui->gridLines->value()));
}

void MainWindow::on_run_clicked()
{
    int N = ui->gridCol->value() * ui->gridLines->value();
    if(ui->type->currentIndex() == 2 && N % 2)         // if the gridsize is even in memory game
    {
        QMessageBox::information(
                    this,
                    tr("Erro na escolha da grade!"),
                    tr("Só é possível escolher um número de cartas pares\n"
                       "no jogo de memória."),
                    QMessageBox::Ok);
        return;
    }

    selTable.setTimeSel(ui->timeSel->value());
    selTable.setText(ui->tableText->text());

    switch (ui->type->currentIndex())
    {
    case 0:  // Normal Grid
    {
        tableView *tb = new tableView(this);
        tb->setTable(selTable);
        tb->setIsLateral(false);

        connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [tb](const EyesPosition &e)
        {
            tb->setPt({
                static_cast<int>(e.gaze.x() * tb->width()),
                static_cast<int>(e.gaze.y() * tb->height())
            });
        });
        connect(tb, SIGNAL(finished(int)), tb, SLOT(deleteLater()));
        connect(tb, &tableView::prevTemplateAsked, [this, tb]()
        {
            skipTemplate(tb, -1);
        });
        connect(tb, &tableView::nextTemplateAsked, [this, tb]()
        {
            skipTemplate(tb, 1);
        });
        tb->exec();
        break;
    }
    case 1:  // Random Grid
    {
        rndTableView *rtb = new rndTableView(this);
        rtb->setTable(selTable);

        connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [rtb](const EyesPosition &e)
        {
            rtb->setPt({
                static_cast<int>(e.gaze.x() * rtb->width()),
                static_cast<int>(e.gaze.y() * rtb->height())
            });
        });
        connect(rtb, SIGNAL(finished(int)), rtb, SLOT(deleteLater()));
        rtb->exec();
        break;
    }
    case 2:  // memory Game
    {
        memoryGame *mg = new memoryGame(this);
        mg->setTable(selTable);
        connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [mg](const EyesPosition &e)
        {
            mg->setPt({
                static_cast<int>(e.gaze.x() * mg->width()),
                static_cast<int>(e.gaze.y() * mg->height())
            });
        });
        connect(mg, SIGNAL(finished(int)), mg, SLOT(deleteLater()));
        mg->exec();
        break;
    }
    case 3:  // double Lateral Grid
    {
        tableView *ltb = new tableView(this);
        ltb->setTable(selTable);
        ltb->setIsLateral(true);

        connect(&_eyetracker, &Eyetracker::eyesPositionChanged, [ltb](const EyesPosition &e)
        {
            ltb->setPt({
                static_cast<int>(e.gaze.x() * ltb->width()),
                static_cast<int>(e.gaze.y() * ltb->height())
            });
        });
        connect(ltb, SIGNAL(finished(int)), ltb, SLOT(deleteLater()));
        connect(ltb, &tableView::prevTemplateAsked, [this, ltb]()
        {
            skipTemplate(ltb, -1);
        });
        connect(ltb, &tableView::nextTemplateAsked, [this, ltb]()
        {
            skipTemplate(ltb, 1);
        });
        ltb->exec();
        break;
    }
    }
}
void MainWindow::loadImagesDir(const QString &pth, QStringList *listImg)
{
    QDir dir(pth);

    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    for (auto &&fileInfo : dir.entryInfoList()) {
        listImg->push_back(QDir().relativeFilePath(fileInfo.filePath()));
    }

    dir.setFilter(QDir::AllDirs | QDir::NoDotDot | QDir::NoDot);
    for (auto &&fileInfo : dir.entryInfoList()) {
        loadImagesDir(fileInfo.filePath(), listImg);
    }

}

void MainWindow::selectImage(int row)
{
    int N = ui->gridLines->value() * ui->gridCol->value();

    if (ui->listSelected->count() >= N && ui->type->currentIndex() != 1)
        return;

    auto item = new QListWidgetItem(ui->listSelected);
    *item = *ui->listImages->item(row);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
    item->setCheckState(Qt::Checked); // AND initialize check state

    ui->listSelected->addItem(item);
    selTable.addImg(imgListPath[row], row);
}

void MainWindow::loadTemplates(QDir dir)
{
    ui->templates->clear();
    templateListPath.clear();

    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::NoSort);  // will sort manually
    dir.setNameFilters({"*.tbl"});

    auto entryList = dir.entryInfoList();
    QCollator collator;
    collator.setNumericMode(true);

    qSort(entryList.begin(), entryList.end(), [&collator](const QFileInfo &fi1, const QFileInfo &fi2)
    {
        return collator.compare(fi1.baseName(), fi2.baseName()) < 0;
    });

    for (auto &&fileInfo : entryList) {
        templateListPath.push_back(fileInfo.filePath());
        ui->templates->addItem(fileInfo.baseName());
    }
}

void MainWindow::on_random_clicked()
{
    int N = ui->gridCol->value() * ui->gridLines->value();
    if (ui->type->currentIndex() == 2 && N % 2)         // if the gridsize is even in memory game
    {
        QMessageBox::information(this,
                                 tr("Erro na escolha da grade!"),
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
    vector<seleItem> selectedPath;

    QString pth = QDir::current().absoluteFilePath("Images");
    if (allImagesPath.isEmpty()) {
        loadImagesDir(pth, &allImagesPath);
    }

    random_shuffle(allImagesPath.begin(), allImagesPath.end());
    if (ui->type->currentIndex() == 2) {   // Memory Game
        for (int i = 0; i < N / 2; i++){
            seleItem it;
            it.path = allImagesPath[i];
            it.id = i;
            selectedPath.push_back(it);
            selectedPath.push_back(it);
        }
    } else {
        for (int i = 0; i < N; i++){
            seleItem it;
            it.path = allImagesPath[i];
            it.id = i;
            selectedPath.push_back(it);
        }
    }

    if(N > selectedPath.size())
        return;

    random_shuffle(selectedPath.begin(), selectedPath.end());
    ui->listSelected->clear();
    selTable.clearAll();
    imgListPath.clear();
    ui->listImages->clear();

    for (int i = 0; i < selectedPath.size(); ++i){
        QFileInfo fileInfo;
        fileInfo.setFile(selectedPath[i].path);
        ui->listSelected->addItem(fileInfo.baseName());
        ui->listSelected->item(i)->setIcon(QIcon(selectedPath[i].path));
        selTable.addImg(selectedPath[i].path, selectedPath[i].id);
        imgListPath.push_back(selectedPath[i].path);
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
    int size = 0;

    // Fake loading only for get Size
    ifp >> type;
    ifp >> cols;
    ifp >> lines;
    ifp >> time;
    ifp.ignore();

    getline(ifp, text);

    QString path;
    do
    {
        string pat;
        int id;
        bool eyeSel;

        getline(ifp, pat);
        ifp >> id;
        ifp >> eyeSel;
        ifp.ignore();

        path = QString::fromLocal8Bit(pat.c_str());
        size++;
    } while (!path.isEmpty());
    ifp.close();

    // Real loading
    // load grid information
    ifp.open(nome.toLocal8Bit());

    ifp >> type;
    ui->type->setCurrentIndex(type);

    ifp >> cols;
    ui->gridCol->setValue(cols);

    ifp >> lines;
    ui->gridLines->setValue(lines);

    ifp >> time;
    ui->timeSel->setValue(time);

    ifp.ignore();
    getline(ifp, text);
    ui->tableText->setText(QString().fromLatin1(text.c_str()));

    // load images
    QProgressDialog progress(tr("Carregando prancha..."), tr("Cancela"), 0, size, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    selTable.clearAll();
    ui->listSelected->clear();
    do
    {
        string pat;
        int id;
        bool eyeSel;

        getline(ifp, pat);
        ifp >> id;
        ifp >> eyeSel;
        ifp.ignore();

        path = QString::fromLocal8Bit(pat.c_str());
        if(!path.isEmpty())
        {
            QFileInfo fileInfo;
            fileInfo.setFile(path);

            ui->listSelected->addItem(fileInfo.baseName());
            int i = ui->listSelected->count() - 1;
            ui->listSelected->item(i)->setIcon(QIcon(path));
            ui->listSelected->item(i)->setFlags(ui->listSelected->item(i)->flags() | Qt::ItemIsUserCheckable);

            selTable.addImg(path,id);
            selTable.setCellEyeSelectable(selTable.count() - 1, eyeSel);

            if (eyeSel)
                ui->listSelected->item(i)->setCheckState(Qt::Checked);
            else
                ui->listSelected->item(i)->setCheckState(Qt::Unchecked);

            progress.setValue(++i);
            QCoreApplication::processEvents();
            if (progress.wasCanceled())
                break;

        }
    } while (!path.isEmpty());

    progress.close();
    ifp.close();
    repaint();
}

void MainWindow::on_actionSalvar_triggered()
{
    QString nome = QFileDialog::getSaveFileName(this, "Select output file to save",
                                                _curDir.absolutePath(), "Tabelas (*.tbl)");
    if (nome.isEmpty())
        return;

    ofstream ofp;
    ofp.open(nome.toLocal8Bit());

    // save grid information
    ofp << ui->type->currentIndex() << endl;
    ofp << ui->gridCol->value() << endl;
    ofp << ui->gridLines->value() << endl;
    ofp << ui->timeSel->value() << endl;
    ofp << ui->tableText->text().toLatin1().constData() << endl;

    // save images paths
    for (int i = 0; i < selTable.count(); i++)
    {
        ofp << selTable.getImgPath(i).toLocal8Bit().toStdString() << endl;
        ofp << selTable.getId(i) << endl;
        ofp << selTable.getCell(i).eyeSelectable << endl;
    }

    ofp.close();
    templateListPath.clear();
    ui->templates->clear();
    loadTemplates(_curDir);
}

void MainWindow::on_actionAbrir_triggered()
{
    QString nome = QFileDialog::getOpenFileName(this,"Select file to open",
                                                "", "Tabelas (*.tbl)");
    if (nome.isEmpty())
        return;

    openTemplate(nome);
}

void MainWindow::on_templates_doubleClicked(const QModelIndex &index)
{
    int i = ui->templates->currentRow();
    openTemplate(templateListPath[i]);
}

void MainWindow::on_clearTable_clicked()
{
    ui->listSelected->clear();
    selTable.clearAll();
    ui->tableText->clear();
    repaint();
}

void MainWindow::on_listSelected_doubleClicked(const QModelIndex &index)
{
    selTable.delImg(index.row());
    ui->listSelected->takeItem(index.row());

}

void MainWindow::on_selecionarTodas_clicked()
{
    const auto size = imgListPath.size();
    QProgressDialog progress(tr("Selecionando imagens..."), tr("Cancela"), 0, size, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    ui->listSelected->clear();
    for (int row = 0; row < size; ++row) {
        selectImage(row);

        progress.setValue(row + 1);
        QApplication::processEvents();
    }
    progress.close();
}
