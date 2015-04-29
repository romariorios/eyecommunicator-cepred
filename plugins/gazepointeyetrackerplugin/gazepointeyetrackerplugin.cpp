#include "gazepointeyetrackerplugin.hpp"

#include <QProcess>
#include <QTimerEvent>

#include <string>

#include "rapidxml.hpp" // Because Qt's XML parser is ASS

using namespace rapidxml;
using namespace std;

GazepointEyetrackerPlugin::GazepointEyetrackerPlugin()
{}

GazepointEyetrackerPlugin::~GazepointEyetrackerPlugin()
{}

QVector<BaseEyetrackerPlugin::Param> GazepointEyetrackerPlugin::availableTrackingParams() const
{
    return {
        { "server", "Server Adderss", Param::String, {}, "127.0.0.1" },
        { "port", "Port", Param::Int, { 1, 65535 }, 4242 },
        { "gazepointExe", "Server executable", Param::Path, {}, {} }
    };
}

bool GazepointEyetrackerPlugin::setTrackingParams(const QVariantHash &params)
{
    _server = params["server"].toString();
    _port = params["port"].toInt();
    _gazepointExe.setProgram(params["gazepointExe"].toString());

    return true;
}

unique_ptr<BaseTrackingCalibrationWidget> GazepointEyetrackerPlugin::trackingCalibrationWidget() const
{
    return nullptr;
}

bool GazepointEyetrackerPlugin::calibrateTracking(const QVector<EyesPosition> &)
{
    _socket.write(valCmd("CALIBRATE_START", true));
    _socket.write(valCmd("CALIBRATE_SHOW", true));
    return _socket.waitForBytesWritten();
}

bool GazepointEyetrackerPlugin::startTracking()
{
    _gazepointExe.start();

    if (!_gazepointExe.waitForStarted())
        return false;

    _socket.connectToHost(_server, _port);
    if (!_socket.waitForConnected()) {
        _gazepointExe.kill();
        return false;
    }


    _socket.write(valCmd("ENABLE_SEND_POG_FIX", true));
    _socket.write(valCmd("ENABLE_SEND_POG_LEFT", true));
    _socket.write(valCmd("ENABLE_SEND_POG_RIGHT", true));
    _socket.write(valCmd("ENABLE_SEND_DATA", true));

    _timerId = startTimer(20);
    return _socket.waitForBytesWritten();
}

bool GazepointEyetrackerPlugin::stopTracking()
{
    killTimer(_timerId);
    _socket.disconnectFromHost();
    _gazepointExe.kill();
    return _socket.waitForDisconnected();
}

template <class Ch>
static double attr_val_d(xml_node<Ch> *node, const char *attr_name)
{
    auto attr = node->first_attribute(attr_name);
    return stod(attr->value());
}

void GazepointEyetrackerPlugin::timerEvent(QTimerEvent *e)
{
    if (e->timerId() != _timerId)
        return QObject::timerEvent(e);

    auto all = _socket.readAll();

    xml_document<> d;
    d.parse<0>(all.data());

    for (auto node = d.first_node("REC"); node; node = node->next_sibling("REC"))
        emit eyesPositionChanged({
            { attr_val_d(node, "FPOGX"), attr_val_d(node, "FPOGY") },
            { attr_val_d(node, "LPOGX"), attr_val_d(node, "LPOGY") },
            { attr_val_d(node, "RPOGX"), attr_val_d(node, "RPOGY") }
        });
}
