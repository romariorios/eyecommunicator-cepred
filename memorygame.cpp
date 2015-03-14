#include "memorygame.h"
#include "ui_memorygame.h"

#include <QtGui>
#include <QSound>
#include <QSvgRenderer>
#include <QLabel>
#include <QMovie>
#include <QDesktopWidget>

memoryGame::memoryGame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::memoryGame)
{
    ui->setupUi(this);
    this->ticTac = new QTimer();
    connect(ticTac, &QTimer::timeout, this, &memoryGame::on_ticTacTimeOver);
    this->setMouseTracking(true);
    movie = new QMovie("iwon.gif");
    this->backGround.load("background.jpg");
    this->cardBack.load("cardBack.png");
    this->selectedSound = new QSound(QDir::currentPath()+"/select.wav");


}

memoryGame::~memoryGame()
{
    delete ui;
    delete this->selectedSound;
    this->ticTac->stop();
    delete this->ticTac;
    delete this->movie;
}

void memoryGame::setTable(table tb)
{
    this->tableData=tb;
}

void memoryGame::setSize()
{

    int w=this->geometry().width();
    int h=this->geometry().height();
    this->movie->setScaledSize(QSize(w,h));
    wi=(float)w/this->tableData.getGridSize().width(); // image size
    hi=(float)h/this->tableData.getGridSize().height();
    if(wi<hi)
        hi=wi=wi*0.8;
    else
        wi=hi=hi*0.8;

}


void memoryGame::paintEvent(QPaintEvent *ev)
{
    if(this->tableData.count()==0)
        return;
    QPainter estojo;
    int w=this->geometry().width();
    int h=this->geometry().height();
    int cols=this->tableData.getGridSize().width();
    int rows=this->tableData.getGridSize().height();
    float dx=(w-(wi*cols))/(cols+1);   //center the images
    float dy=(h-(hi*rows))/(rows+1);
    float x=dx;
    float y=dy;
    QPixmap im=this->tableData.getImage(0);
    QRectF pos(x,y,wi,hi);
    estojo.begin(this);
    QRectF tam2(0,0,this->backGround.width(),this->backGround.height());
    estojo.drawPixmap(QRectF(0,0,w,h),this->backGround,tam2);
    for(int i=0;i<this->tableData.count();i++)
    {
        im=this->tableData.getImage(i);
        if(i==i1st || i==i2nd || matched[i])
        {
            QRectF tam(0,0,im.width(),im.height());
            estojo.drawPixmap(pos,im,tam);
        }
        else
        {
            QRectF tam(0,0,this->cardBack.width(),this->cardBack.height());
            estojo.drawPixmap(pos,this->cardBack,tam);
        }
        x+=(wi+dx);
        if(x>=w-1-dx)
        {
            x=dx;
            y+=(hi+dy);
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
    QDesktopWidget *widget = QApplication::desktop();
    int nSrc=widget->screenCount();
    QRect rect = widget->screenGeometry(nSrc-1);             // get second window size

    this->setGeometry(rect);

    this->ticTac->start(this->tableData.getTimeSel()*100);
    this->sizeSel=0;
    this->rowSelOld=-1;
    this->i1st=this->i2nd=-1;
    this->memoTime=0;
    this->memoStart=false;

    this->matched.clear();
    this->matched.assign(this->tableData.count(),false);

    this->setSize();
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
    this->close();
}

void memoryGame::on_ticTacTimeOver()
{
    int w=this->geometry().width();
    int h=this->geometry().height();
    int cols=this->tableData.getGridSize().width();
    int rows=this->tableData.getGridSize().height();

    float dx=(w-(wi*cols))/(cols+1);   //center the images
    float dy=(h-(hi*rows))/(rows+1);
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
                        this->selectedSound->play();
                    }
                    else if(i2nd<0) // selecting the second card
                    {
                        if(i!=i1st) // desconsidering double selecting the same card
                        {
                            this->i2nd=i;
                            this->selectedSound->play();
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

                }
            }
        }
        x+=(wi+dx);
        if(x>=w-1-dx)
        {
            x=dx;
            y+=(hi+dy);
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
        this->fini();
    }
    this->repaint();

}


void memoryGame::fini()
{
    QLabel *l=new QLabel();
    if(!movie->isValid())
        int u=0;
    l->setMovie(movie);
    this->ui->verticalLayout->addWidget(l);
    this->setSize();
    movie->setSpeed(200);
    QSound::play("iwon.wav");
    movie->start();
    this->repaint();

}
