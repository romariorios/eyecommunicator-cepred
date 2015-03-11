#ifndef TABLE_H
#define TABLE_H
#include <vector>
#include <QStringList>
#include <QPixmap>
using namespace std;

class tableCell
{
public:
    QPixmap img;
    QString imgPath;
    int id;
};

class table
{
public:
    table();
    ~table();
    void addImg(QString imgPth);
    void addImg(QString imgPth,int id);

    void delImg(int row);
    void clearAll();
    QSize getGridSize() const;
    void setGridSize(const QSize &value);
    QPixmap getImage(int row);
    int count(){return this->cell.size();}

    int getGridType() const;
    void setGridType(int value);

    double getTimeSel() const;
    void setTimeSel(double value);

    int getId(int row){return this->cell[row].id;}

private:
    int gridType;
    QSize gridSize;
    vector <tableCell> cell;
    double timeSel;
};

#endif // TABLE_H
