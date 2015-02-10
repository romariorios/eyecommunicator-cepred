#ifndef TABLE_H
#define TABLE_H
#include <vector>
#include <QStringList>
#include <QPixmap>
using namespace std;


class table
{
public:
    table();
    ~table();
    void addImg(QString imgPth);
    void delImg(int row);
    QSize getGridSize() const;
    void setGridSize(const QSize &value);
    QPixmap getImage(int row);
    int count(){return this->imgVec.size();}

    int getGridType() const;
    void setGridType(int value);

    double getTimeSel() const;
    void setTimeSel(double value);

private:
    int gridType;
    QSize gridSize;
    vector <QPixmap> imgVec;
    QStringList imgPath;
    double timeSel;
};

#endif // TABLE_H
