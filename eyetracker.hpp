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

    void addPluginPath(const QDir &dir); // adiciona pasta dos plugins
    void setCurrentPlugin(int index); // seleciona plugin
    QVector<BaseEyetrackerPlugin::Param> availableParams() const;
    inline QVariantHash currentParams() const { return _currentParams; }

    unique_ptr<BaseTrackingCalibrationWidget> calibrationWidget() const;
    bool calibrate(const QVector<EyesPosition> &map);

    QVector<QString> pluginsFound() const;
    inline bool isPluginSet() const { return _curPlugin; } // verifica se existe um plugin selecionado
    inline int curPluginIndex() const { return _curPluginIndex; }

public slots:
    bool start(const QVariantHash &availableParams);
    bool start();
    bool stop();

signals:
    void eyesPositionChanged(const EyesPosition &pos);

private:
    deque<QPluginLoader> _pluginsFound;
    BaseEyetrackerPlugin *_curPlugin = nullptr;
    int _curPluginIndex = -1; // relies on _pluginsFound never having an item removed
    bool _hasStarted = false;
    QVariantHash _currentParams;
};

#endif // EYETRACKER_HPP
