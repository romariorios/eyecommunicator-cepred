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
    this->imgPath.push_back(imgPth);

    if(imgPth.right(3)=="svg")
    {
        QPixmap px(800,800);
        px.fill(QColor(255,255,255));
        QPainter pai(&px);
        QSvgRenderer renderer(imgPth);
        renderer.render(&pai);
        this->imgVec.push_back(px);

    }
    else
    {
        this->imgVec.push_back(QPixmap(imgPth));
    }
}

void table::delImg(int row)
{
    this->imgPath.erase(imgPath.begin()+row);
    this->imgVec.erase(imgVec.begin()+row);
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
    return this->imgVec[row];
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




