#include "table.h"
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>

table::table()
{

}

table::~table()
{

}

void table::addImg(QString imgPth)
{
    tableCell c;
    c.imgPath=imgPth;
    c.id=this->cell.size();

    if(imgPth.right(3)=="svg")
    {
        QPixmap px(800,800);
        px.fill(QColor(255,255,255));
        QPainter pai(&px);
        QSvgRenderer renderer(imgPth);
        renderer.render(&pai);
        c.img=px;

    }
    else
    {
        c.img=QPixmap(imgPth);
    }
    this->cell.push_back(c);
}

void table::addImg(QString imgPth, int id)
{
    tableCell c;
    c.imgPath=imgPth;
    c.id=id;

    if(imgPth.right(3)=="svg")
    {
        QPixmap px(800,800);
        px.fill(QColor(255,255,255));
        QPainter pai(&px);
        QSvgRenderer renderer(imgPth);
        renderer.render(&pai);
        c.img=px;

    }
    else
    {
        c.img=QPixmap(imgPth);
    }
    this->cell.push_back(c);

}

void table::delImg(int row)
{
    this->cell.erase(cell.begin()+row);
}

void table::clearAll()
{
    this->cell.clear();
}
QSize table::getGridSize() const
{
    return gridSize;
}

void table::setGridSize(const QSize &value)
{
    gridSize = value;
}

QPixmap table::getImage(int row)
{
    return this->cell[row].img;
}
int table::getGridType() const
{
    return gridType;
}

void table::setGridType(int value)
{
    gridType = value;
}
double table::getTimeSel() const
{
    return timeSel;
}

void table::setTimeSel(double value)
{
    timeSel = value;
}




