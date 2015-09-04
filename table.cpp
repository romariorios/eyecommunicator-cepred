#include "table.h"
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QFileInfo>

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
    c.eyeSelectable=true;

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

void table::addCell(tableCell c)
{
    this->cell.push_back(c);
}

tableCell table::getCell(int i)
{
    if(i<this->cell.size())
        return this->cell[i];
}

void table::setCellEyeSelectable(int i, bool val)
{
    this->cell[i].eyeSelectable=val;
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

void table::randomImages()
{
    random_shuffle(this->cell.begin(),this->cell.end());
}
QString table::getText() const
{
    return text;
}

void table::setText(const QString &value)
{
    text = value;
}







QString tableCell::getImgName()
{
    QFileInfo fileInfo;
    fileInfo.setFile(this->imgPath);
    return fileInfo.baseName();

}
