#ifndef MEMORYGAME_H
#define MEMORYGAME_H
#include <QDialog>
#include "table.h"
#include <vector>
using namespace std;

namespace Ui {
class memoryGame;
}

class memoryGame : public QDialog
{
    Q_OBJECT

public:
    explicit memoryGame(QWidget *parent = 0);
    ~memoryGame();
    void setTable(table tb);

private:
    Ui::memoryGame *ui;
    table tableData;        // the images data
    QPoint pt;              // the eyetracker point
    QTimer *ticTac;
    QRectF posSel;           // the selected image position

    double sizeSel;         // size of the animation selection procsess
    int rowSelOld;          // the last image index selected
    float wi,hi;            // size of images in the grid
    int i1st;               // index of first card selected
    int i2nd;               // index of second card selected
    vector <bool> matched;  // vector of the cards matched
    double memoTime;        // timesteps to memorizing wrong cards
    bool memoStart;         // iniciates timesteps for memorizing wrong cards
    void finished();        // Wow the game!
    QMovie *movie;

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

#endif // MEMORYGAME_H
