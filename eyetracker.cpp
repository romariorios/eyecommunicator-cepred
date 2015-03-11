#include "eyetracker.hpp"

#include <QPluginLoader>

#include "pluginconfigdialog.hpp"

Eyetracker::Eyetracker(QObject *parent) :
    QObject(parent)
{
}

void Eyetracker::addPluginPath(const QDir &dir)
{
    for (auto f : dir.entryList(QDir::Files)) {
        auto absPath = dir.absoluteFilePath(f);
        QPluginLoader loader{absPath};

        if (loader.load()) {
            loader.unload();
            _pluginsFound.emplace_back(absPath);
        }
    }

    if (_pluginsFound.size() == 1) {
        setCurrentPlugin(0);
    } else {
        setCurrentPlugin(-1);
    }
}

void Eyetracker::setCurrentPlugin(int index)
{
    if (_curPluginIndex == index) {
        return;
    }

    if (_curPlugin) {
        disconnect(
            _curPlugin->qObject(), SIGNAL(eyesPositionChanged(EyesPosition)),
            this, SIGNAL(eyesPositionChanged(EyesPosition)));

        _curPlugin->stopTracking();
        _pluginsFound[_curPluginIndex].unload();
    }

    if (index < 0 || index >= _pluginsFound.size()) {
        _curPlugin = nullptr;
        _curPluginIndex = -1;

        return;
    }

    _curPlugin = qobject_cast<BaseEyetrackerPlugin *>(_pluginsFound[index].instance());
    _curPluginIndex = index;

    connect(
        _curPlugin->qObject(), SIGNAL(eyesPositionChanged(EyesPosition)),
        this, SIGNAL(eyesPositionChanged(EyesPosition)));
}

QVector<BaseEyetrackerPlugin::Param> Eyetracker::params() const
{
    if (!_curPlugin) {
        return {};
    }

    return _curPlugin->availableTrackingParams();
}

bool Eyetracker::start(const QVariantHash &params)
{
    if (_hasStarted && !_curPlugin->stopTracking())
        return false;

    return _hasStarted = _curPlugin &&
        _curPlugin->setTrackingParams(params) &&
        _curPlugin->startTracking();
}

bool Eyetracker::start()
{
    if (!_curPlugin)
        return false;

    PluginConfigDialog d{params()};

    return d.exec() && start(d.result());
}

bool Eyetracker::stop()
{
    if (!_curPlugin || !_hasStarted || !_curPlugin->stopTracking())
        return false;

    _hasStarted = false;
    return true;
}

unique_ptr<BaseTrackingCalibrationWidget> Eyetracker::calibrationWidget() const
{
    if (!_curPlugin) {
        return unique_ptr<BaseTrackingCalibrationWidget>{};
    }

    return _curPlugin->trackingCalibrationWidget();
}

bool Eyetracker::calibrate(const QVector<EyesPosition> &map)
{
    return _curPlugin && _curPlugin->calibrateTracking(map);
}

QVector<QString> Eyetracker::pluginsFound() const
{
    QVector<QString> res;

    for (auto &p : _pluginsFound) {
        auto metadata = p.metaData()["MetaData"].toObject();
        res << metadata["displayName"].toString();
    }

    return res;
}


