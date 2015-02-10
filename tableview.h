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
    void setTable(table tb);

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

protected:
    void paintEvent(QPaintEvent *ev);
    void showEvent(QShowEvent *sev);
    void closeEvent(QCloseEvent *cev);
    void hideEvent(QHideEvent *hev);

public slots:
    inline setPt(const QPoint &p) { pt = p; }

private slots:
    void on_ticTacTimeOver();

};

#endif // TABLEVIEW_H
