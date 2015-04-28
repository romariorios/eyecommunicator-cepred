#ifndef DUMMYEYETRACKERPLUGIN_HPP
#define DUMMYEYETRACKERPLUGIN_HPP

#include <baseeyetrackerplugin.hpp>

#include <QTcpSocket>

class EyeTribeEyetrackerPlugin : public QObject, public BaseEyetrackerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(
        IID "br.ufba.dcc.eyecommunicator-cepred.plugins.eyetribe"
        FILE "eyetribeeyetrackerplugin.json")

    EYETRACKER_PLUGIN_INTERFACE(EyeTribe)

signals:
    void eyesPositionChanged(const EyesPosition &pos);

private:
    void reqFrames();

    int _port;
    bool _ended = false;
    QTcpSocket _socket;
};

#endif // DUMMYEYETRACKERPLUGIN_HPP
