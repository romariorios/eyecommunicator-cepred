#include "table.h"
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QFileInfo>

void table::addImg(const QString &imgPth)
{
    tableCell c;
    c.imgPath = imgPth;
    c.id = cell.size();

    if (imgPth.right(3) == "svg")
    {
        QPixmap px(800, 800);
        px.fill(QColor(255, 255, 255));
        QPainter pai(&px);
        QSvgRenderer renderer(imgPth);
        renderer.render(&pai);
        c.img = px;
    }
    else
    {
        c.img = QPixmap(imgPth);
    }
    cell.push_back(c);
}

void table::addImg(const QString &imgPth, int id)
{
    tableCell c;
    c.imgPath = imgPth;
    c.id = id;
    c.eyeSelectable = true;

    if (imgPth.right(3) == "svg")
    {
        QPixmap px(800, 800);
        px.fill(QColor(255, 255, 255));
        QPainter pai(&px);
        QSvgRenderer renderer(imgPth);
        renderer.render(&pai);
        c.img = px;
    }
    else
    {
        c.img = QPixmap(imgPth);
    }
    cell.push_back(c);

}

void table::addCell(const tableCell &c)
{
    cell.push_back(c);
}

const tableCell &table::getCell(int i) const
{
    if (i < cell.size())
        return cell[i];
}

void table::setCellEyeSelectable(int i, bool val)
{
    cell[i].eyeSelectable = val;
}

void table::delImg(int row)
{
    cell.erase(cell.begin()+row);
}

void table::clearAll()
{
    cell.clear();
}

QSize table::getGridSize() const
{
    return gridSize;
}

void table::setGridSize(const QSize &value)
{
    gridSize = value;
}

QPixmap table::getImage(int row) const
{
    return cell[row].img;
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
    random_shuffle(cell.begin(), cell.end());
}

QString table::getText() const
{
    return text;
}

void table::setText(const QString &value)
{
    text = value;
}

QString tableCell::getImgName() const
{
    QFileInfo fileInfo;
    fileInfo.setFile(imgPath);
    return fileInfo.baseName();
}
