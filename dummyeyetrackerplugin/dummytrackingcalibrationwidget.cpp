#include "dummytrackingcalibrationwidget.hpp"
#include "ui_dummytrackingcalibrationwidget.h"

DummyTrackingCalibrationWidget::DummyTrackingCalibrationWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DummyTrackingCalibrationWidget)
{
    ui->setupUi(this);
}

DummyTrackingCalibrationWidget::~DummyTrackingCalibrationWidget()
{
    delete ui;
}


QVector<EyesPosition> DummyTrackingCalibrationWidget::calibrationResult() const
{
    return {};
}
