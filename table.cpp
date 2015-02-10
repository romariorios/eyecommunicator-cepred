#include "table.h"

table::table()
{

}

table::~table()
{

}

void table::addImg(QString imgPth)
{
    this->imgPath.push_back(imgPth);
    this->imgVec.push_back(QPixmap(imgPth));
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




