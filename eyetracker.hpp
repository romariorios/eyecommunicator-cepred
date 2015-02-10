#ifndef EYETRACKER_HPP
#define EYETRACKER_HPP

#include <QObject>

#include <deque>

#include <QDir>
#include <QPluginLoader>
#include <QVector>

#include "baseeyetrackerplugin.hpp"

using std::deque;

class Eyetracker : public QObject
{
    Q_OBJECT
public:
    explicit Eyetracker(QObject *parent = 0);

    void addPluginPath(const QDir &dir);
    void setCurrentPlugin(int index);
    QVector<BaseEyetrackerPlugin::Param> params() const;

    unique_ptr<BaseTrackingCalibrationWidget> calibrationWidget() const;
    bool calibrate(const QVector<EyesPosition> &map);

    QVector<QString> pluginsFound() const;
    inline bool isPluginSet() const { return _curPlugin; }

public slots:
    bool start(const QVariantHash &params);
    bool start();
    bool stop();

signals:
    void eyesPositionChanged(const EyesPosition &pos);

private:
    deque<QPluginLoader> _pluginsFound;
    BaseEyetrackerPlugin *_curPlugin = nullptr;
    int _curPluginIndex = -1; // relies on _pluginsFound never having an item removed
    bool _hasStarted = false;
};

#endif // EYETRACKER_HPP
