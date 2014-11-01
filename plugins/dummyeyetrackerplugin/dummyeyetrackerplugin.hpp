#ifndef DUMMYEYETRACKERPLUGIN_HPP
#define DUMMYEYETRACKERPLUGIN_HPP

#include <baseeyetrackerplugin.hpp>

class DummyEyetrackerPlugin : public QObject, public BaseEyetrackerPlugin
{
    Q_OBJECT
    Q_INTERFACES(BaseEyetrackerPlugin)
    Q_PLUGIN_METADATA(
        IID "br.ufba.dcc.eyecommunicator-cepred.plugins.dummy"
        FILE "dummyeyetrackerplugin.json")

public:
    DummyEyetrackerPlugin();
    ~DummyEyetrackerPlugin();

    QVector<Param> availableTrackingParams() const;
    bool setTrackingParams(const QVariantHash &params);

    unique_ptr<BaseTrackingCalibrationWidget> trackingCalibrationWidget() const;
    bool calibrateTracking(const QVector<EyesPosition> &map);

    bool startTracking();
    bool stopTracking();

protected:
    void timerEvent(QTimerEvent *e);

signals:
    void eyesPositionChanged(const EyesPosition &pos);

private:
    int _period;
    int _curTimerId = 0;
};

#endif // DUMMYEYETRACKERPLUGIN_HPP
