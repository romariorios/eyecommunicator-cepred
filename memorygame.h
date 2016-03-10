#ifndef MEMORYGAME_H
#define MEMORYGAME_H
#include <QDialog>
#include "table.h"
#include <vector>
#include <QPixmap>
#include <QSound>
#include <QTimer>
#include <QMovie>

using namespace std;

#include "ui_memorygame.h"

class memoryGame : public QDialog
{
    Q_OBJECT

public:
    explicit memoryGame(QWidget *parent = 0);
    ~memoryGame();
    void setTable(table tb);
    void setSize();

private:
    Ui::memoryGame ui;
    table tableData;        // the images data
    QPoint pt;              // the eyetracker point
    QTimer ticTac;
    QRectF posSel;           // the selected image position

    double sizeSel;         // size of the animation selection procsess
    int rowSelOld;          // the last image index selected
    float wi,hi;            // size of images in the grid
    int i1st;               // index of first card selected
    int i2nd;               // index of second card selected
    vector <bool> matched;  // vector of the cards matched
    double memoTime;        // timesteps to memorizing wrong cards
    bool memoStart;         // iniciates timesteps for memorizing wrong cards
    void fini();        // Wow the game!
    QMovie movie{"iwon.gif"};
    QPixmap backGround;     // backgroung image
    QPixmap cardBack;       // back of the cards in memory game
    QSound selectedSound;        // sound of selected image

protected:
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void hideEvent(QHideEvent *);

public slots:
    inline void setPt(const QPoint &p) { pt = p; }

private slots:
    void on_ticTacTimeOver();

};

#endif // MEMORYGAME_H
