#include "tableview.h"
#include "ui_tableview.h"
#include <QtGui>
#include <QSound>
#include <QSvgRenderer>
#include <QDesktopWidget>


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
void tableView::setIsLateral(bool value)
{
    isLateral = value;
}


void tableView::paintEvent(QPaintEvent *ev)
{
    if(this->tableData.count()==0)
        return;
    QPainter estojo;
    float fwi=wi;
    float fhi=hi;
    if(this->isLateral)
    {
        fwi=fwi*0.7;
        fhi=fhi*0.7;
    }
    int w=this->geometry().width();
    int h=this->geometry().height();
    int cols=tableData.getGridSize().width();
    int rows=tableData.getGridSize().height();
    float dx=(w-(fwi*cols))/(cols+1);   //center the images
    float dy=(h-(fhi*rows))/(rows+1);
    float x;
    float y=dy;
    if(this->isLateral)
    {
        x=1;
        dx=3*dx;
    }
    else
        x=dx;

    QPixmap im=tableData.getImage(0);
    QRectF pos(x,y,fwi,fhi);
    estojo.begin(this);
    //    QRectF tam2(0,0,backGround.width(),backGround.height());
    //    estojo.drawPixmap(QRectF(0,0,w,h),backGround,tam2);
    for(int i=0;i<tableData.count();i++)
    {
        im=tableData.getImage(i);
        QRectF tam(0,0,im.width(),im.height());
        estojo.drawPixmap(pos,im,tam);
        x+=(fwi+dx);
        if(x>=w-1-dx && !isLateral)
        {
            x=dx;
            y+=(fhi+dy);
        }
        pos=QRectF(x,y,fwi,fhi);

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

    //estojo.drawEllipse(this->posSel.center(),this->sizeSel*fwi/2,this->sizeSel*fhi/2);
    estojo.end();

}

void tableView::on_ticTacTimeOver()
{
    if(this->sizeSel>=1)    // the image was aready selected
        return;
    float fwi=wi;
    float fhi=hi;
    if(this->isLateral)
    {
        fwi=fwi*0.7;
        fhi=fhi*0.7;
    }
    int w=this->geometry().width();
    int h=this->geometry().height();
    int cols=tableData.getGridSize().width();
    int rows=tableData.getGridSize().height();
    float dx=(w-(fwi*cols))/(cols+1);   //center the images
    float dy=(h-(fhi*rows))/(rows+1);
    float x;
    float y=dy;
    if(this->isLateral)
    {
        x=1;
        dx=3*dx;
    }
    else
        x=dx;
    QRectF pos(x,y,fwi,fhi);
    this->posSel = {-1,-1,-1,-1};       // not found
    for(int i=0;i<this->tableData.count();i++)
    {
        if(pos.contains(pt) && this->tableData.getCell(i).eyeSelectable) // mudança aqui
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
                QSound::play(QDir::currentPath()+"/select.wav");
                this->ticTac->stop();
            }
            this->repaint();
            return;
        }

        x+=(fwi+dx);
        if(x>=w-1-dx && !isLateral)
        {
            x=dx;
            y+=(fhi+dy);
        }
        pos=QRectF(x,y,fwi,fhi);
    }
    this->repaint();

}

void tableView::showEvent(QShowEvent *sev)
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
    this->ui->label->setText(this->tableData.getText());
}


void tableView::setSize()
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

void tableView::keyPressEvent(QKeyEvent *key)
{
    if(key->key()==Qt::Key_Delete)
    {
        this->sizeSel=0;
        this->rowSelOld=-1;
        this->selected=false;
        this->ticTac->start(this->tableData.getTimeSel()*100);
    }
    QDialog::keyPressEvent(key);
    return;

}


