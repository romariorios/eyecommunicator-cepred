#include "dummyeyetrackerplugin.hpp"
#include "dummytrackingcalibrationwidget.hpp"

DummyEyetrackerPlugin::DummyEyetrackerPlugin()
{}

QVector<BaseEyetrackerPlugin::Param> DummyEyetrackerPlugin::availableTrackingParams() const
{
    return { { "period", "Period" } };
}

bool DummyEyetrackerPlugin::setTrackingParams(const QVariantHash &params)
{
    _period = params["period"].value<long>();
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

}

bool DummyEyetrackerPlugin::stopTracking()
{
}
