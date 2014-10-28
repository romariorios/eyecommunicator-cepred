#ifndef BASECALIBRATIONWIDGET_HPP
#define BASECALIBRATIONWIDGET_HPP

#include <QWidget>

#include <QPointF>
#include <QVector>

#include <eyesposition.hpp>

class BaseTrackingCalibrationWidget : public QWidget
{
public:
    virtual QVector<EyesPosition> calibrationResult() const = 0;
};

#endif // BASECALIBRATIONWIDGET_HPP
