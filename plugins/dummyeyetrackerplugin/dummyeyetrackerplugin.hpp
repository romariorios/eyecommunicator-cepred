#ifndef DUMMYEYETRACKERPLUGIN_HPP
#define DUMMYEYETRACKERPLUGIN_HPP

#include <baseeyetrackerplugin.hpp>

class DummyEyetrackerPlugin : public QObject, public BaseEyetrackerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(
        IID "br.ufba.dcc.eyecommunicator-cepred.plugins.dummy"
        FILE "dummyeyetrackerplugin.json")

    EYETRACKER_PLUGIN_INTERFACE(Dummy)

signals:
    void eyesPositionChanged(const EyesPosition &pos);

protected:
    void timerEvent(QTimerEvent *e);

private:
    int _period;
    int _curTimerId = 0;
};

#endif // DUMMYEYETRACKERPLUGIN_HPP
