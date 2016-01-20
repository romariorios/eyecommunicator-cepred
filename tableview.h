#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QDialog>
#include "table.h"

namespace Ui {
class tableView;
}

class tableView : public QDialog
{
    Q_OBJECT

public:
    explicit tableView(QWidget *parent = 0);
    ~tableView();
    void setTable(const table &tb);

    void setIsLateral(bool value);

private:
    Ui::tableView *ui;
    table tableData;        // the images data
    QPoint pt;              // the eyetracker point
    QTimer *ticTac;
    QRectF posSel;           // the selected image position
    double sizeSel;         // size of the animation selection procsess
    int rowSelOld;          // the last image index selected
    float wi,hi;            // size of images in the grid
    bool selected;
    bool isLateral;         // indicates that the grid is double lateral type
    void setSize();

protected:
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void hideEvent(QHideEvent *);
    void keyPressEvent(QKeyEvent *key);

public slots:
    inline setPt(const QPoint &p) { pt = p; }

private slots:
    void on_ticTacTimeOver();

};

#endif // TABLEVIEW_H
