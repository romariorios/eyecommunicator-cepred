#ifndef DUMMYEYETRACKERPLUGIN_HPP
#define DUMMYEYETRACKERPLUGIN_HPP

#include <baseeyetrackerplugin.hpp>

#include <QProcess>
#include <QTcpSocket>

class GazepointEyetrackerPlugin : public QObject, public BaseEyetrackerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(
        IID "br.ufba.dcc.eyecommunicator-cepred.plugins.gazepoint"
        FILE "gazepointeyetrackerplugin.json")

    EYETRACKER_PLUGIN_INTERFACE(Gazepoint)

signals:
    void eyesPositionChanged(const EyesPosition &pos);

protected:
    void timerEvent(QTimerEvent *e);

private:
    enum class Cmd { GET, SET };

    inline QByteArray xmlCmd(Cmd type, const QString &cmd, const QString &params = {}) const
    {
        return
            (QString{"<"} + (type == Cmd::GET? "GET" : "SET") +
            " ID=\"" + cmd + "\" " + params + ">\r\n").toLocal8Bit();
    }
    inline QByteArray valCmd(const QString &cmd, const QVariant &val) const
    {
        auto isBool = val.type() == QVariant::Bool;

        return xmlCmd(
            Cmd::SET,
            cmd,
            (isBool? "STATE" : "VALUE") +
            QString{"=\""} +
            (isBool? val.toBool()? "1" : "0" : val.toString()) + "\"");
    }

    QTcpSocket _socket;
    QString _server;
    QProcess _gazepointExe;
    int _port;
    int _timerId;
};

#endif // DUMMYEYETRACKERPLUGIN_HPP
