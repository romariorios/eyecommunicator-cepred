#ifndef DUMMYEYETRACKERPLUGIN_HPP
#define DUMMYEYETRACKERPLUGIN_HPP

#include "../baseeyetrackerplugin.hpp"

#include <dummytrackingcalibrationwidget.hpp>

class DummyEyetrackerPlugin : public BaseEyetrackerPlugin, public QObject
{
    Q_OBJECT

public:
    DummyEyetrackerPlugin();
    ~DummyEyetrackerPlugin();

    QVector<Param> availableTrackingParams() const;
    bool setTrackingParams(const QVariantHash &params);

    unique_ptr<BaseTrackingCalibrationWidget> trackingCalibrationWidget() const;
    bool calibrateTracking(const QVector<EyesPosition> &map);

    bool startTracking();
    bool stopTracking();

signals:
    void eyesPositionChanged(const EyesPosition &pos);

private:
    long _period;
    DummyTrackingCalibrationWidget _calibrationWidget;
};

#endif // DUMMYEYETRACKERPLUGIN_HPP
