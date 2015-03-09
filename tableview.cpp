#include "tableview.h"
#include "ui_tableview.h"
#include <QtGui>
#include <QSound>
#include <QSvgRenderer>

tableView::tableView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tableView)
{
    ui->setupUi(this);
    this->ticTac = new QTimer();
    connect(ticTac, &QTimer::timeout, this, &tableView::on_ticTacTimeOver);
    this->setMouseTracking(true);
}

tableView::~tableView()
{
    delete ui;
    this->ticTac->stop();
}

void tableView::setTable(table tb)
{
    this->tableData=tb;
}

void tableView::paintEvent(QPaintEvent *ev)
{
    if(this->tableData.count()==0)
        return;
    QPainter estojo;
    int w=this->geometry().width();
    int h=this->geometry().height();
    float dx=(w-(wi*this->tableData.getGridSize().width()))/2;   //center the images
    float dy=(h-(hi*this->tableData.getGridSize().height()))/2;
    float x=dx;
    float y=dy;
    QPixmap im=this->tableData.getImage(0);
    QRectF pos(x,y,wi,hi);
    QRectF tam(0,0,im.width(),im.height());
    estojo.begin(this);
    estojo.setBrush(QColor(200,200,200));
    estojo.drawRect(0,0,w,h);

    for(int i=0;i<this->tableData.count();i++)
    {
        im=this->tableData.getImage(i);
        QRectF tam(0,0,im.width(),im.height());
        estojo.drawPixmap(pos,im,tam);
        x+=wi;
        if(x>=w-1-dx)
        {
            x=dx;
            y+=hi;
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

void tableView::showEvent(QShowEvent *sev)
{
    this->ticTac->start(this->tableData.getTimeSel()*100);
    this->sizeSel=0;
    this->rowSelOld=-1;
    int w=this->geometry().width();
    int h=this->geometry().height();
    wi=(float)w/this->tableData.getGridSize().width();
    hi=(float)h/this->tableData.getGridSize().height();
    if(wi<hi)
        hi=wi;
    else
        wi=hi;
    this->selected=false;
}

void tableView::closeEvent(QCloseEvent *cev)
{
    this->ticTac->stop();
    this->disconnect();

}

void tableView::hideEvent(QHideEvent *hev)
{
    this->ticTac->stop();
    this->disconnect();
}

void tableView::on_ticTacTimeOver()
{
    if(this->sizeSel>=1)    // the image was aready selected
        return;
    int w=this->geometry().width();
    int h=this->geometry().height();
    float dx=(w-(wi*this->tableData.getGridSize().width()))/2;   //center the images
    float dy=(h-(hi*this->tableData.getGridSize().height()))/2;
    float x=dx;
    float y=dy;
    QRectF pos(x,y,wi,hi);
    this->posSel=QRectF(-1,-1,-1,-1);       // not found
    for(int i=0;i<this->tableData.count();i++)
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
                QSound::play(QDir::currentPath()+"/Images/select.wav");
                this->ticTac->stop();
            }
            this->repaint();
            return;
        }
        x+=wi;
        if(x>=w-1-dx)
        {
            x=dx;
            y+=hi;
        }
        pos=QRectF(x,y,wi,hi);
    }
    this->repaint();

}
