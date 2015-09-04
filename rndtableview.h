#ifndef RNDTABLEVIEW_H
#define RNDTABLEVIEW_H

#include <QDialog>
#include "table.h"
#include <QSound>
#include <QStringList>


namespace Ui {
class rndTableView;
}

class rndTableView : public QDialog
{
    Q_OBJECT

public:
    explicit rndTableView(QWidget *parent = 0);
    ~rndTableView();
    void setTable(table tb);

private:
    Ui::rndTableView *ui;
    table tableData;        // the images data
    table rndTableData;     // random selected images
    QPoint pt;              // the eyetracker point
    QTimer *ticTac;
    QRectF posSel;           // the selected image position
    double sizeSel;         // size of the animation selection procsess
    int rowSelOld;          // the last image index selected
    float wi,hi;            // size of images in the grid
    bool selected;
    void setSize();
    bool rndSelimages();    // method to select images from tableData to rndTableData
    QSound selectedSound;        // sound of selected image
    QStringList excPair1;   // Exclusion Pairs of images
    QStringList excPair2;
    int actualItem;
    void openExcludedPairs(QString name);
    bool filterOK(int id);




protected:
    void paintEvent(QPaintEvent *ev);
    void showEvent(QShowEvent *sev);
    void closeEvent(QCloseEvent *cev);
    void hideEvent(QHideEvent *hev);
    void keyPressEvent(QKeyEvent *key);

public slots:
    inline setPt(const QPoint &p) { pt = p; }

private slots:
    void on_ticTacTimeOver();

};

#endif // TABLEVIEW_H
