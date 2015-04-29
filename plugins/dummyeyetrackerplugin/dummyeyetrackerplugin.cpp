#include "dummyeyetrackerplugin.hpp"
#include "dummytrackingcalibrationwidget.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QTimerEvent>

DummyEyetrackerPlugin::DummyEyetrackerPlugin()
{}

DummyEyetrackerPlugin::~DummyEyetrackerPlugin()
{}

QVector<BaseEyetrackerPlugin::Param> DummyEyetrackerPlugin::availableTrackingParams() const
{
    return {
        { "period", "Period (ms)", Param::Int, { 10 , 500 } },
        { "test1", "This will not affect anything", Param::Float, { 0.1, 0.9 } },
        { "test2", "This will also not affect anything", Param::String },
        { "test3", "This", Param::Set, { "Will", "not", "affect", "anything" } }
    };
}

bool DummyEyetrackerPlugin::setTrackingParams(const QVariantHash &params)
{
    _period = params["period"].toInt();

    return true;
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

    return true;
}

bool DummyEyetrackerPlugin::stopTracking()
{
    killTimer(_curTimerId);
    _curTimerId = 0;

    return true;
}

void DummyEyetrackerPlugin::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == _curTimerId) {
        auto pos = QCursor::pos();
        auto screen = QApplication::desktop()->screenGeometry();
        QPointF normPos{
            double{pos.x()} / double{screen.width()},
            double{pos.y()} / double{screen.height()}
        };

        emit eyesPositionChanged({normPos, normPos, normPos});
    }
}
