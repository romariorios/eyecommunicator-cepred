#ifndef BASECALIBRATIONWIDGET_HPP
#define BASECALIBRATIONWIDGET_HPP

#include <QVector>
#include <eyesposition.hpp>

class BaseTrackingCalibrationWidget
{
public:
    virtual QVector<EyesPosition> calibrationResult() const = 0;
};

#endif // BASECALIBRATIONWIDGET_HPP
