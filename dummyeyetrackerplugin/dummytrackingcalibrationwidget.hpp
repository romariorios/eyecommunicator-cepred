#ifndef DUMMYTRACKINGCALIBRATIONWIDGET_HPP
#define DUMMYTRACKINGCALIBRATIONWIDGET_HPP

#include "../basetrackingcalibrationwidget.hpp"
#include <QDialog>

namespace Ui {
class DummyTrackingCalibrationWidget;
}

class DummyTrackingCalibrationWidget :
    public BaseTrackingCalibrationWidget,
    public QDialog
{
    Q_OBJECT

public:
    explicit DummyTrackingCalibrationWidget(QWidget *parent = 0);
    ~DummyTrackingCalibrationWidget();

    QVector<EyesPosition> calibrationResult() const;

private:
    Ui::DummyTrackingCalibrationWidget *ui;
};

#endif // DUMMYTRACKINGCALIBRATIONWIDGET_HPP
