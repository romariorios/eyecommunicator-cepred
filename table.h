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
    QString getImgName() const;
    int id;
    bool eyeSelectable;
};

class table
{
public:
    void addImg(const QString &imgPth);
    void addImg(const QString &imgPth, int id);
    void addCell(const tableCell &c);
    const tableCell &getCell(int i) const;
    void setCellEyeSelectable(int i,bool val);

    void delImg(int row);
    void clearAll();
    QSize getGridSize() const;
    void setGridSize(const QSize &value);
    QPixmap getImage(int row) const;
    int count() const { return cell.size(); }

    int getGridType() const;
    void setGridType(int value);

    double getTimeSel() const;
    void setTimeSel(double value);

    int getId (int row) const { return cell[row].id; }
    QString getImgPath(int row) const { return cell[row].imgPath; }
    void randomImages();

    QString getText() const;
    void setText(const QString &value);

private:
    int gridType;
    QSize gridSize;
    vector <tableCell> cell;
    double timeSel;
    QString text;
};

#endif // TABLE_H
