#include "eyetracker.hpp"

#include <QPluginLoader>

Eyetracker::Eyetracker(QObject *parent) :
    QObject(parent)
{
}

void Eyetracker::addPluginPath(const QDir &dir)
{
    for (auto f : dir.entryList(QDir::Files)) {
        auto absPath = dir.absoluteFilePath(f);
        QPluginLoader loader{dir.absoluteFilePath(f)};
        auto plugin = qobject_cast<BaseEyetrackerPlugin *>(loader.instance());

        if (plugin) {
            loader.unload();
            _pluginsFound.emplace_back(absPath);
        }
    }

    if (_pluginsFound.size() == 1) {
        setCurrentPlugin(0);
    }
}

void Eyetracker::setCurrentPlugin(int index)
{
    if (_curPluginIndex == index) {
        return;
    }

    if (_curPlugin) {
        _pluginsFound[_curPluginIndex].unload();
        _curPlugin->stopTracking();
    }

    if (index < 0 || index >= _pluginsFound.size()) {
        _curPlugin = nullptr;
        _curPluginIndex = -1;

        return;
    }

    _curPlugin = qobject_cast<BaseEyetrackerPlugin *>(_pluginsFound[index].instance());
    _curPluginIndex = index;
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
    return _curPlugin &&
        _curPlugin->setTrackingParams(params) &&
            _curPlugin->startTracking();
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
        res << p.metaData()["displayName"].toString();
    }

    return res;
}


