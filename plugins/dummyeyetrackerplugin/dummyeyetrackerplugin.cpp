#include "dummyeyetrackerplugin.hpp"
#include "dummytrackingcalibrationwidget.hpp"

#include <QTimerEvent>

DummyEyetrackerPlugin::DummyEyetrackerPlugin()
{}

DummyEyetrackerPlugin::~DummyEyetrackerPlugin()
{}

QVector<BaseEyetrackerPlugin::Param> DummyEyetrackerPlugin::availableTrackingParams() const
{
    return { { "period", "Period", BaseEyetrackerPlugin::Param::Int, { 0 , 500 } } };
}

bool DummyEyetrackerPlugin::setTrackingParams(const QVariantHash &params)
{
    _period = params["period"].toInt();
}

unique_ptr<BaseTrackingCalibrationWidget> DummyEyetrackerPlugin::trackingCalibrationWidget() const
{
    // TODO use make_unique when available
    return unique_ptr<BaseTrackingCalibrationWidget>
        { new DummyTrackingCalibrationWidget };
}

bool DummyEyetrackerPlugin::calibrateTracking(const QVector<EyesPosition> &map)
{
    return true;
}

bool DummyEyetrackerPlugin::startTracking()
{
    _curTimerId = startTimer(_period);
}

bool DummyEyetrackerPlugin::stopTracking()
{
    killTimer(_curTimerId);
    _curTimerId = 0;
}

void DummyEyetrackerPlugin::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == _curTimerId) {
        emit eyesPositionChanged({{1.0, 1.0}, {1.0, 1.0}, {1.0, 1.0}});
    }
}
