#include "tableview.h"
#include "ui_tableview.h"
#include <QtGui>
#include <QSound>
#include <QSvgRenderer>
#include <QDesktopWidget>

tableView::tableView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tableView)
{
    ui->setupUi(this);
    ticTac = new QTimer();
    connect(ticTac, &QTimer::timeout, this, &tableView::on_ticTacTimeOver);
    setMouseTracking(true);
}

tableView::~tableView()
{
    delete ui;
    ticTac->stop();
}

void tableView::setTable(const table &tb)
{
    tableData = tb;
    resetState();
}

void tableView::setIsLateral(bool value)
{
    isLateral = value;
}

void tableView::paintEvent(QPaintEvent *)
{
    if (tableData.count() == 0)
        return;

    QPainter estojo;
    float fwi = wi;
    float fhi = hi;

    if (isLateral)
    {
        fwi = fwi * 0.7;
        fhi = fhi * 0.7;
    }

    int w = geometry().width();
    int h = geometry().height();
    int cols = tableData.getGridSize().width();
    int rows = tableData.getGridSize().height();
    float dx = (w-(fwi * cols)) / (cols + 1);   //center the images
    float dy = (h-(fhi * rows)) / (rows + 1);
    float x;
    float y = dy;

    if (isLateral)
    {
        x = 1;
        dx = 3 * dx;
    }
    else
        x = dx;

    QPixmap im = tableData.getImage(0);
    QRectF pos(x, y, fwi, fhi);
    estojo.begin(this);
    estojo.setRenderHints(
                QPainter::Antialiasing |
                QPainter::SmoothPixmapTransform);
    for (int i = 0; i<tableData.count(); i++)
    {
        im = tableData.getImage(i);
        QRectF tam(0, 0, im.width(), im.height());
        estojo.drawPixmap(pos, im, tam);
        x += fwi + dx;
        if (x >= w - 1 - dx && !isLateral)
        {
            x = dx;
            y += (fhi+dy);
        }
        pos = QRectF(x, y, fwi, fhi);

    }
    if (selected)
    {
        estojo.setBrush(QColor(0, 255, 0, 80));
        estojo.drawRect(posSel);
    }
    else
    {
        if (posSel.x()>=0 && posSel.y()>=0)
        {
            estojo.setBrush(QColor(0, 255, 0, 30));
            estojo.drawRect(posSel);
        }
    }

    estojo.end();
}

void tableView::on_ticTacTimeOver()
{
    if (sizeSel >= 1)    // the image was aready selected
        return;

    float fwi = wi;
    float fhi = hi;

    if (isLateral)
    {
        fwi = fwi * 0.7;
        fhi = fhi * 0.7;
    }

    int w = geometry().width();
    int h = geometry().height();
    int cols = tableData.getGridSize().width();
    int rows = tableData.getGridSize().height();
    float dx = (w - (fwi * cols)) / (cols + 1);   //center the images
    float dy = (h - (fhi * rows)) / (rows + 1);
    float x;
    float y = dy;

    if (isLateral)
    {
        x = 1;
        dx = 3 * dx;
    }
    else
        x = dx;

    QRectF pos(x, y, fwi, fhi);
    posSel = {-1, -1, -1, -1};       // not found
    for (int i = 0; i<tableData.count(); i++)
    {
        if (pos.contains(pt) && tableData.getCell(i).eyeSelectable) // mudança aqui
        {
            posSel = pos;

            if (rowSelOld == i)
            {
                sizeSel += 0.10;
            }
            else
                sizeSel = 0;

            rowSelOld = i;
            if (sizeSel>=0.99999)
            {
                selected = true;
                QSound::play(QDir::currentPath()+"/select.wav");
                ticTac->stop();
            }
            repaint();
            return;
        }

        x += fwi + dx;

        if (x >= w - 1 - dx && !isLateral)
        {
            x = dx;
            y += fhi + dy;
        }

        pos = QRectF(x, y, fwi, fhi);
    }
    repaint();

}

void tableView::showEvent(QShowEvent *)
{
    QDesktopWidget *widget = QApplication::desktop();
    int nSrc = widget->screenCount();
    QRect rect = widget->screenGeometry(nSrc - 1);             // get second window size

    setGeometry(rect);
    resetState();
}


void tableView::setSize()
{
    int w = geometry().width();
    int h = geometry().height();

    wi = static_cast<float>(w) / tableData.getGridSize().width(); // image size
    hi = static_cast<float>(h) / tableData.getGridSize().height();

    if (wi < hi)
        hi = wi = wi * 0.8;
    else
        wi = hi = hi * 0.8;
}

void tableView::resetState()
{
    ticTac->start(tableData.getTimeSel() * 100);
    sizeSel = 0;
    rowSelOld = -1;
    setSize();
    selected = false;
    ui->label->setText(tableData.getText());
}

void tableView::closeEvent(QCloseEvent *)
{
    ticTac->stop();
    disconnect();
}

void tableView::hideEvent(QHideEvent *)
{
    ticTac->stop();
    disconnect();
}

void tableView::keyPressEvent(QKeyEvent *key)
{
    switch (key->key())
    {
    case Qt::Key_Delete:
        sizeSel = 0;
        rowSelOld = -1;
        selected = false;
        ticTac->start(tableData.getTimeSel() * 100);
        break;
    case Qt::Key_Left:
        emit prevTemplateAsked();
        break;
    case Qt::Key_Right:
        emit nextTemplateAsked();
        break;
    default:
        QDialog::keyPressEvent(key);
    }
}
