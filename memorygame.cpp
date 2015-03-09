#include "memorygame.h"
#include "ui_memorygame.h"

#include <QtGui>
#include <QSound>
#include <QSvgRenderer>
#include <QLabel>
#include <QMovie>

memoryGame::memoryGame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::memoryGame)
{
    ui->setupUi(this);
    this->ticTac = new QTimer();
    connect(ticTac, &QTimer::timeout, this, &memoryGame::on_ticTacTimeOver);
    this->setMouseTracking(true);
    movie = new QMovie("iwon.gif");

}

memoryGame::~memoryGame()
{
    delete ui;
    this->ticTac->stop();
}

void memoryGame::setTable(table tb)
{
    this->tableData=tb;
}


void memoryGame::paintEvent(QPaintEvent *ev)
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
        if(i==i1st || i==i2nd || matched[i])
            estojo.drawPixmap(pos,im,tam);
        else
            estojo.drawRect(pos);
        x+=wi;
        if(x>=w-1-dx)
        {
            x=dx;
            y+=hi;
        }
        pos=QRectF(x,y,wi,hi);

    }
    if(this->posSel.x()>=0 && this->posSel.y()>=0)
    {
        estojo.setBrush(QColor(0,255,0,30));
        estojo.drawRect(this->posSel);
    }



    //estojo.drawEllipse(this->posSel.center(),this->sizeSel*wi/2,this->sizeSel*hi/2);
    estojo.end();

}

void memoryGame::showEvent(QShowEvent *sev)
{
    this->ticTac->start(this->tableData.getTimeSel()*100);
    this->sizeSel=0;
    this->rowSelOld=-1;
    this->i1st=this->i2nd=-1;
    this->memoTime=0;
    this->memoStart=false;

    int w=this->geometry().width();
    int h=this->geometry().height();
    this->movie->setScaledSize(QSize(w,h));
    wi=(float)w/this->tableData.getGridSize().width();
    hi=(float)h/this->tableData.getGridSize().height();
    if(wi<hi)
        hi=wi;
    else
        wi=hi;
    this->matched.clear();
    this->matched.assign(this->tableData.count(),false);
}

void memoryGame::closeEvent(QCloseEvent *cev)
{
    this->ticTac->stop();
    this->disconnect();
}

void memoryGame::hideEvent(QHideEvent *hev)
{
    this->ticTac->stop();
    this->disconnect();
}

void memoryGame::on_ticTacTimeOver()
{
    int w=this->geometry().width();
    int h=this->geometry().height();
    float dx=(w-(wi*this->tableData.getGridSize().width()))/2;   //center the images
    float dy=(h-(hi*this->tableData.getGridSize().height()))/2;
    float x=dx;
    float y=dy;
    QRectF pos(x,y,wi,hi);
    this->posSel=QRectF(-1,-1,-1,-1);       // not found
    int nMatched=0;
    for(int i=0;i<this->tableData.count();i++)
    {
        if(this->matched[i])nMatched++;         // count matched cards
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
                this->sizeSel=0;
                if(!this->matched[i])       // only avaliable cards
                {
                    if(i1st<0)    // selecting the first card
                    {
                        this->i1st=i;
                    }
                    else if(i2nd<0) // selecting the second card
                    {
                        if(i!=i1st) // desconsidering double selecting
                        {
                            this->i2nd=i;
                            // verify if was matched
                            if(this->tableData.getId(i1st)==this->tableData.getId(i2nd))
                            {
                                this->matched[i1st]=this->matched[i2nd]=true;
                                i1st=i2nd=-1;
                            }
                            else
                                memoStart=true; // let the image while to memorize
                        }

                    }

                    QSound::play(QDir::currentPath()+"/Images/select.wav");
                }
            }
        }
        x+=wi;
        if(x>=w-1-dx)
        {
            x=dx;
            y+=hi;
        }
        pos=QRectF(x,y,wi,hi);
    }
    if(memoStart)
    {
        memoTime+=0.2;
        if(memoTime>=0.9999)
        {
            this->memoTime=0;
            i1st=i2nd=-1;
            memoStart=false;
        }
    }
    if(nMatched==this->tableData.count())       // end of game
    {
        this->ticTac->stop();
        this->finished();
    }
    this->repaint();

}


void memoryGame::finished()
{
    QLabel *l=new QLabel();
    if(!movie->isValid())
        int u=0;
    l->setMovie(movie);
    this->ui->verticalLayout->addWidget(l);
    movie->start();
    this->repaint();

}
