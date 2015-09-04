#include "rndtableview.h"
#include "ui_rndtableview.h"
#include <QtGui>
#include <QSound>
#include <QSvgRenderer>
#include <QDesktopWidget>
#include <fstream>


rndTableView::rndTableView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rndTableView),
    selectedSound(QDir::currentPath() + "/selectSimple.wav")
{
    ui->setupUi(this);
    this->ticTac = new QTimer();
    connect(ticTac, &QTimer::timeout, this, &rndTableView::on_ticTacTimeOver);
    this->setMouseTracking(true);
    this->openExcludedPairs("exclusaoPares.txt");
    this->actualItem=0;

}

rndTableView::~rndTableView()
{
    delete ui;
    this->ticTac->stop();
}

void rndTableView::setTable(table tb)
{
    this->tableData=tb;
    this->rndSelimages();
}

void rndTableView::paintEvent(QPaintEvent *ev)
{
    if(this->tableData.count()==0)
        return;
    QPainter estojo;
    int w=this->geometry().width();
    int h=this->geometry().height();
    int cols=tableData.getGridSize().width();
    int rows=tableData.getGridSize().height();
    float dx=(w-(wi*cols))/(cols+1);   //center the images
    float dy=(h-(hi*rows))/(rows+1);
    float x=dx;
    float y=dy;
    QPixmap im=rndTableData.getImage(0);
    QRectF pos(x,y,wi,hi);
    estojo.begin(this);
    //    QRectF tam2(0,0,backGround.width(),backGround.height());
    //    estojo.drawPixmap(QRectF(0,0,w,h),backGround,tam2);
    for(int i=0;i<rndTableData.count();i++)
    {
        im=rndTableData.getImage(i);
        QRectF tam(0,0,im.width(),im.height());
        estojo.drawPixmap(pos,im,tam);
        x+=(wi+dx);
        if(x>=w-1-dx)
        {
            x=dx;
            y+=(hi+dy);
        }
        pos=QRectF(x,y,wi,hi);

    }
    if(this->selected)
    {
        estojo.setBrush(QColor(0,255,0,80));
        estojo.drawRect(this->posSel);
    }
    else
    {
        if(this->posSel.x()>=0 && this->posSel.y()>=0)
        {
            estojo.setBrush(QColor(0,255,0,30));
            estojo.drawRect(this->posSel);
        }
    }

    //estojo.drawEllipse(this->posSel.center(),this->sizeSel*wi/2,this->sizeSel*hi/2);
    estojo.end();

}

void rndTableView::on_ticTacTimeOver()
{
    if(this->sizeSel>=1)    // the image was aready selected
        return;
    int w=this->geometry().width();
    int h=this->geometry().height();
    int cols=tableData.getGridSize().width();
    int rows=tableData.getGridSize().height();
    float dx=(w-(wi*cols))/(cols+1);   //center the images
    float dy=(h-(hi*rows))/(rows+1);
    float x=dx;
    float y=dy;
    QRectF pos(x,y,wi,hi);
    this->posSel = {-1,-1,-1,-1};       // not found
    for(int i=0;i<this->rndTableData.count();i++)
    {
        if(pos.contains(pt))
        {
            posSel=pos;
            if(rowSelOld==i)
            {
                sizeSel+=0.10;
            }
            else
                sizeSel=0;
            rowSelOld=i;
            if(sizeSel>=0.99999)
            {
                this->selected=true;
                selectedSound.play();
                this->ticTac->stop();
            }
            this->repaint();
            return;
        }

        x+=(wi+dx);
        if(x>=w-1-dx)
        {
            x=dx;
            y+=(hi+dy);
        }
        pos=QRectF(x,y,wi,hi);
    }
    this->repaint();

}

void rndTableView::showEvent(QShowEvent *sev)
{
    QDesktopWidget *widget = QApplication::desktop();
    int nSrc=widget->screenCount();
    QRect rect = widget->screenGeometry(nSrc-1);             // get second window size

    setGeometry(rect);

    this->ticTac->start(this->tableData.getTimeSel()*100);
    this->sizeSel=0;
    this->rowSelOld=-1;
    this->setSize();
    this->selected=false;

}


void rndTableView::setSize()
{

    int w=geometry().width();
    int h=geometry().height();
    wi=(float)w/tableData.getGridSize().width(); // image size
    hi=(float)h/tableData.getGridSize().height();
    if(wi<hi)
        hi=wi=wi*0.8;
    else
        wi=hi=hi*0.8;

}

bool rndTableView::rndSelimages()
{
    int cols=tableData.getGridSize().width();
    int rows=tableData.getGridSize().height();
    int N= cols*rows;
    int nT=this->tableData.getListCellSize();
    this->rndTableData.clearAll();
    this->rndTableData.addCell(this->tableData.getCell(this->actualItem));
    int i=0;
    int tries=0;
    while(i<N-1 && tries<1000)
    {
        tries++;
        int id=rand()%nT;
        if(this->filterOK(id))
        {
            i++;
            this->rndTableData.addCell(this->tableData.getCell(id));
        }
    }
    this->rndTableData.randomImages();
    this->actualItem++;
    if(this->actualItem>=nT)
        this->actualItem=0;

    return true;
}

void rndTableView::openExcludedPairs(QString name)
{
    ifstream ifp;
    ifp.open(name.toLocal8Bit());
    string p1,p2;
    this->excPair1.clear();
    this->excPair2.clear();
    while(ifp>>p1>>p2)
    {
        this->excPair1.push_back(QString::fromLatin1(p1.c_str()));
        this->excPair2.push_back(QString::fromLatin1(p2.c_str()));
    }
    ifp.close();
}

bool rndTableView::filterOK(int id)
{
    if(actualItem==id)
        return false;
    QString actName=this->tableData.getCell(this->actualItem).getImgName();
    QString tableName=this->tableData.getCell(id).getImgName();
    // Verify if the same image was previously selected
    int tam=this->rndTableData.getListCellSize();
    for(int i=0;i<tam;i++)
    {
        QString rndTableName=this->rndTableData.getCell(i).getImgName();
        if(rndTableName==tableName)
            return false;
    }

    // verify exclusion pairs
   for(int i=0;i<this->excPair1.size();i++)
   {

       if(tableName==this->excPair1[i] && actName==this->excPair2[i] ||
          tableName==this->excPair2[i] && actName==this->excPair1[i])
           return false;
   }
   return true;
}

void rndTableView::closeEvent(QCloseEvent *cev)
{
    this->ticTac->stop();
    this->disconnect();

}

void rndTableView::hideEvent(QHideEvent *hev)
{
    this->ticTac->stop();
    this->disconnect();
}

void rndTableView::keyPressEvent(QKeyEvent *key)
{

    if(key->key()==Qt::Key_Delete)
    {
        this->sizeSel=0;
        this->rowSelOld=-1;
        this->selected=false;
        this->ticTac->start(this->tableData.getTimeSel()*100);
    }
    if(key->key()==Qt::Key_Space)
    {
        this->rndSelimages();
        this->sizeSel=0;
        this->rowSelOld=-1;
        this->selected=false;
        this->ticTac->start(this->tableData.getTimeSel()*100);
        this->repaint();
    }

    QDialog::keyPressEvent(key);
    return;

}


