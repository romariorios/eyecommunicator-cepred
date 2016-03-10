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
    ticTac = new QTimer();
    connect(ticTac, &QTimer::timeout, this, &rndTableView::on_ticTacTimeOver);
    setMouseTracking(true);
    openExcludedPairs("exclusaoPares.txt");
    actualItem=0;

}

rndTableView::~rndTableView()
{
    delete ui;
    ticTac->stop();
}

void rndTableView::setTable(table tb)
{
    tableData=tb;
    rndSelimages();
}

void rndTableView::paintEvent(QPaintEvent *)
{
    if (tableData.count() == 0)
        return;
    QPainter estojo;
    int w=geometry().width();
    int h=geometry().height();
    int cols=tableData.getGridSize().width();
    int rows=tableData.getGridSize().height();
    float dx=(w-(wi*cols))/(cols+1);   //center the images
    float dy=(h-(hi*rows))/(rows+1);
    float x=dx;
    float y=dy;
    QPixmap im=rndTableData.getImage(0);
    QRectF pos(x,y,wi,hi);
    estojo.begin(this);
    estojo.setRenderHints(
                QPainter::Antialiasing |
                QPainter::SmoothPixmapTransform);
    //    QRectF tam2(0,0,backGround.width(),backGround.height());
    //    estojo.drawPixmap(QRectF(0,0,w,h),backGround,tam2);
    for (int i=0;i<rndTableData.count();i++)
    {
        im=rndTableData.getImage(i);
        QRectF tam(0,0,im.width(),im.height());
        estojo.drawPixmap(pos,im,tam);
        x+=(wi+dx);
        if (x>=w-1-dx)
        {
            x=dx;
            y+=(hi+dy);
        }
        pos=QRectF(x,y,wi,hi);

    }
    if (selected)
    {
        estojo.setBrush(QColor(0,255,0,80));
        estojo.drawRect(posSel);
    }
    else
    {
        if (posSel.x()>=0 && posSel.y()>=0)
        {
            estojo.setBrush(QColor(0,255,0,30));
            estojo.drawRect(posSel);
        }
    }

    //estojo.drawEllipse(posSel.center(),sizeSel*wi/2,sizeSel*hi/2);
    estojo.end();

}

void rndTableView::on_ticTacTimeOver()
{
    if (sizeSel>=1)    // the image was aready selected
        return;
    int w=geometry().width();
    int h=geometry().height();
    int cols=tableData.getGridSize().width();
    int rows=tableData.getGridSize().height();
    float dx=(w-(wi*cols))/(cols+1);   //center the images
    float dy=(h-(hi*rows))/(rows+1);
    float x=dx;
    float y=dy;
    QRectF pos(x,y,wi,hi);
    posSel = {-1,-1,-1,-1};       // not found
    for (int i=0;i<rndTableData.count();i++)
    {
        if (pos.contains(pt))
        {
            posSel=pos;
            if (rowSelOld == i)
            {
                sizeSel+=0.10;
            }
            else
                sizeSel=0;
            rowSelOld=i;
            if (sizeSel>=0.99999)
            {
                selected=true;
                selectedSound.play();
                ticTac->stop();
            }
            repaint();
            return;
        }

        x+=(wi+dx);
        if (x>=w-1-dx)
        {
            x=dx;
            y+=(hi+dy);
        }
        pos=QRectF(x,y,wi,hi);
    }
    repaint();

}

void rndTableView::showEvent(QShowEvent *)
{
    QDesktopWidget *widget = QApplication::desktop();
    int nSrc=widget->screenCount();
    QRect rect = widget->screenGeometry(nSrc-1);             // get second window size

    setGeometry(rect);

    ticTac->start(tableData.getTimeSel()*100);
    sizeSel=0;
    rowSelOld=-1;
    setSize();
    selected=false;

}


void rndTableView::setSize()
{

    int w=geometry().width();
    int h=geometry().height();
    wi=(float)w/tableData.getGridSize().width(); // image size
    hi=(float)h/tableData.getGridSize().height();
    if (wi<hi)
        hi=wi=wi*0.8;
    else
        wi=hi=hi*0.8;

}

bool rndTableView::rndSelimages()
{
    int cols=tableData.getGridSize().width();
    int rows=tableData.getGridSize().height();
    int N= cols*rows;
    int nT=tableData.count();
    rndTableData.clearAll();
    rndTableData.addCell(tableData.getCell(actualItem));
    int i=0;
    int tries=0;
    while(i<N-1 && tries<1000)
    {
        tries++;
        int id=rand()%nT;
        if (filterOK(id))
        {
            i++;
            rndTableData.addCell(tableData.getCell(id));
        }
    }
    rndTableData.randomImages();
    actualItem++;
    if (actualItem>=nT)
        actualItem=0;

    return true;
}

void rndTableView::openExcludedPairs(const QString &name)
{
    ifstream ifp;
    ifp.open(name.toLocal8Bit());
    string p1,p2;
    excPair1.clear();
    excPair2.clear();
    while(ifp>>p1>>p2)
    {
        excPair1.push_back(QString::fromLatin1(p1.c_str()));
        excPair2.push_back(QString::fromLatin1(p2.c_str()));
    }
    ifp.close();
}

bool rndTableView::filterOK(int id)
{
    if (actualItem == id)
        return false;
    QString actName=tableData.getCell(actualItem).getImgName();
    QString tableName=tableData.getCell(id).getImgName();
    // Verify if the same image was previously selected
    int tam = rndTableData.count();
    for (int i=0;i<tam;i++)
    {
        QString rndTableName=rndTableData.getCell(i).getImgName();
        if (rndTableName == tableName)
            return false;
    }

    // verify exclusion pairs
   for (int i=0;i<excPair1.size();i++)
   {

       if (tableName == excPair1[i] && actName == excPair2[i] ||
          tableName == excPair2[i] && actName == excPair1[i])
           return false;
   }
   return true;
}

void rndTableView::closeEvent(QCloseEvent *)
{
    ticTac->stop();
    disconnect();

}

void rndTableView::hideEvent(QHideEvent *)
{
    ticTac->stop();
    disconnect();
}

void rndTableView::keyPressEvent(QKeyEvent *key)
{

    if (key->key() == Qt::Key_Delete)
    {
        sizeSel=0;
        rowSelOld=-1;
        selected=false;
        ticTac->start(tableData.getTimeSel()*100);
    }
    if (key->key() == Qt::Key_Space)
    {
        rndSelimages();
        sizeSel=0;
        rowSelOld=-1;
        selected=false;
        ticTac->start(tableData.getTimeSel()*100);
        repaint();
    }

    QDialog::keyPressEvent(key);
    return;

}


