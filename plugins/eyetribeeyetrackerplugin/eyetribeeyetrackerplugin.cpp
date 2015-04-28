#include "eyetribeeyetrackerplugin.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>

using namespace std;

EyeTribeEyetrackerPlugin::EyeTribeEyetrackerPlugin()
{}

EyeTribeEyetrackerPlugin::~EyeTribeEyetrackerPlugin()
{
    _ended = true;
}

QVector<BaseEyetrackerPlugin::Param> EyeTribeEyetrackerPlugin::availableTrackingParams() const
{
    return {{"port", "Port", Param::Int, {0, 65535}, 6555}};
}

bool EyeTribeEyetrackerPlugin::setTrackingParams(const QVariantHash &params)
{
    _port = params["port"].toInt();
    return true;
}

unique_ptr<BaseTrackingCalibrationWidget> EyeTribeEyetrackerPlugin::trackingCalibrationWidget() const
{
    return nullptr;
}

bool EyeTribeEyetrackerPlugin::calibrateTracking(const QVector<EyesPosition> &)
{
    return true;
}

QPointF mkPoint(const QJsonObject &obj)
{
    return {obj.value("x").toDouble(), obj.value("y").toDouble()};
}

bool EyeTribeEyetrackerPlugin::startTracking()
{
    _socket.connectToHost(QHostAddress::LocalHost, _port);
    if (!_socket.waitForConnected(50))
        return false;

    reqFrames();
    if (!_socket.waitForBytesWritten(50)) {
        _socket.disconnectFromHost();
        _socket.waitForDisconnected();
        return false;
    }

    connect(&_socket, &QIODevice::readyRead, [this]()
    {
        auto &&jsonDoc = QJsonDocument::fromJson(_socket.readAll());
        if (!jsonDoc.isObject())
            return;

        auto &&json = jsonDoc.object();
        if (
            json.value("category").toString() != "tracker" ||
            json.value("request").toString() != "get")
            return;

        auto &&frame = json.value("values").toObject().value("frame").toObject();
        auto getEye = [frame](const char *eye)
        {
            return frame.value(eye).toObject().value("pcenter").toObject();
        };
        auto &&leftEye = getEye("lefteye");
        auto &&rightEye = getEye("righteye");
        auto &&avgEyes = frame.value("avg").toObject();

        auto &&screen = QApplication::desktop()->screenGeometry();
        auto &&avgEyesRaw = mkPoint(avgEyes);
        QPointF avgEyesPt = {
            avgEyesRaw.x() / screen.width(),
            avgEyesRaw.y() / screen.height()};

        emit eyesPositionChanged({avgEyesPt, mkPoint(leftEye), mkPoint(rightEye)});
    });

    connect(&_socket, &QAbstractSocket::stateChanged, [this](QAbstractSocket::SocketState s)
    {
        if (!_ended && s == QAbstractSocket::UnconnectedState) {
            _socket.connectToHost(QHostAddress::LocalHost, _port);
            if (!_socket.waitForConnected())
                return;

            reqFrames();
        }
    });

    return true;
}

bool EyeTribeEyetrackerPlugin::stopTracking()
{
    _socket.disconnectFromHost();
    return _socket.waitForDisconnected();
}

void EyeTribeEyetrackerPlugin::reqFrames()
{
    _socket.write(R"({"values":{"push":true,"version":1},"category":"tracker","request":"set"})");
}
