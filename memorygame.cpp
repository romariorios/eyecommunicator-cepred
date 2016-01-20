#include "memorygame.h"

#include <QtGui>
#include <QSound>
#include <QSvgRenderer>
#include <QLabel>
#include <QMovie>
#include <QDesktopWidget>

memoryGame::memoryGame(QWidget *parent) :
    QDialog(parent),
    selectedSound(QDir::currentPath() + "/select.wav")
{
    ui.setupUi(this);
    connect(&ticTac, &QTimer::timeout, this, &memoryGame::on_ticTacTimeOver);
    setMouseTracking(true);
    backGround.load("background.jpg");
    cardBack.load("cardBack.png");
}

memoryGame::~memoryGame()
{
    ticTac.stop();
}

void memoryGame::setTable(table tb)
{
    tableData = tb;
}

void memoryGame::setSize()
{
    int w = geometry().width();
    int h = geometry().height();

    movie.setScaledSize(QSize(w, h));
    wi = static_cast<float>(w) / tableData.getGridSize().width(); // image size
    hi = static_cast<float>(h) / tableData.getGridSize().height();
    if (wi < hi)
        hi = wi = wi * 0.8;
    else
        wi = hi = hi * 0.8;
}


void memoryGame::paintEvent(QPaintEvent *)
{
    if (tableData.count() == 0)
        return;

    QPainter estojo;
    int w = geometry().width();
    int h = geometry().height();
    int cols = tableData.getGridSize().width();
    int rows = tableData.getGridSize().height();
    float dx = (w - (wi * cols)) / (cols + 1);   //center the images
    float dy = (h - (hi * rows)) / (rows + 1);
    float x = dx;
    float y = dy;
    QPixmap im = tableData.getImage(0);
    QRectF pos(x, y, wi, hi);

    estojo.begin(this);
    QRectF tam2(0, 0, backGround.width(), backGround.height());
    estojo.drawPixmap(QRectF(0, 0, w, h), backGround, tam2);

    for (int i = 0; i < tableData.count(); i++)
    {
        im = tableData.getImage(i);
        if (i == i1st || i == i2nd || matched[i])
        {
            QRectF tam(0, 0, im.width(), im.height());
            estojo.drawPixmap(pos, im, tam);
        }
        else
        {
            QRectF tam(0, 0, cardBack.width(), cardBack.height());
            estojo.drawPixmap(pos,cardBack,tam);
        }
        x += wi + dx;
        if (x >= w - 1 - dx)
        {
            x = dx;
            y += hi + dy;
        }
        pos = QRectF(x, y, wi, hi);

    }
    if (posSel.x() >= 0 && posSel.y() >= 0)
    {
        estojo.setBrush(QColor(0, 255, 0, 30));
        estojo.drawRect(posSel);
    }

    estojo.end();
}

void memoryGame::showEvent(QShowEvent *)
{
    QDesktopWidget *widget = QApplication::desktop();
    int nSrc = widget->screenCount();
    QRect rect = widget->screenGeometry(nSrc - 1);             // get second window size

    setGeometry(rect);

    ticTac.start(tableData.getTimeSel() * 100);
    sizeSel = 0;
    rowSelOld =- 1;
    i1st=i2nd =- 1;
    memoTime = 0;
    memoStart = false;

    matched.clear();
    matched.assign(tableData.count(), false);

    setSize();
}

void memoryGame::closeEvent(QCloseEvent *)
{
    ticTac.stop();
    disconnect();
}

void memoryGame::hideEvent(QHideEvent *)
{
    ticTac.stop();
    disconnect();
    close();
}

void memoryGame::on_ticTacTimeOver()
{
    int w = geometry().width();
    int h = geometry().height();
    int cols = tableData.getGridSize().width();
    int rows = tableData.getGridSize().height();
    float dx = (w - (wi * cols)) / (cols + 1);   //center the images
    float dy = (h - (hi * rows)) / (rows + 1);
    float x = dx;
    float y = dy;

    QRectF pos(x, y, wi, hi);
    posSel = QRectF(-1, -1, -1, -1);       // not found
    int nMatched = 0;

    for(int i = 0; i < tableData.count(); i++)
    {
        if (matched[i])
            nMatched++;         // count matched cards

        if (pos.contains(pt))
        {
            posSel = pos;

            if (rowSelOld == i)
                sizeSel += 0.10;
            else
                sizeSel = 0;

            rowSelOld = i;

            if (sizeSel >= 0.99999)
            {
                sizeSel = 0;
                if (!matched[i])       // only avaliable cards
                {
                    if (i1st < 0)    // selecting the first card
                    {
                        i1st = i;
                        selectedSound.play();
                    }
                    else if (i2nd < 0) // selecting the second card
                    {
                        if (i != i1st) // desconsidering double selecting the same card
                        {
                            i2nd = i;
                            selectedSound.play();

                            // verify if was matched
                            if (tableData.getId(i1st) == tableData.getId(i2nd))
                            {
                                matched[i1st] = matched[i2nd] = true;
                                i1st = i2nd = -1;
                            }
                            else
                                memoStart = true; // let the image while to memorize
                        }

                    }

                }
            }
        }

        x += wi + dx;
        if (x >= w - 1 - dx)
        {
            x = dx;
            y += hi+dy;
        }

        pos = QRectF(x, y, wi, hi);
    }

    if (memoStart)
    {
        memoTime += 0.2;
        if (memoTime >= 0.9999)
        {
            memoTime = 0;
            i1st = i2nd = -1;
            memoStart = false;
        }
    }

    if (nMatched == tableData.count())       // end of game
    {
        ticTac.stop();
        fini();
    }

    repaint();

}


void memoryGame::fini()
{
    QLabel *l = new QLabel(this);
    l->setMovie(&movie);
    ui.verticalLayout->addWidget(l);

    setSize();
    movie.setSpeed(200);
    QSound::play("iwon.wav");
    movie.start();
    repaint();
}
