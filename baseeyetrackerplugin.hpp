#ifndef BASEEYETRACKERPLUGIN_HPP
#define BASEEYETRACKERPLUGIN_HPP

#include <QHash>
#include <QString>
#include <QVariant>
#include <QVector>

#include <memory>

#include "basetrackingcalibrationwidget.hpp"
#include <eyesposition.hpp>

using std::unique_ptr;

class BaseEyetrackerPlugin
{
public:
    struct Param
    {
        QString name;
        QString fullName;
    };

    virtual ~BaseEyetrackerPlugin() {}

    virtual QVector<Param> availableTrackingParams() const = 0;
    virtual bool setTrackingParams(const QVariantHash &params) = 0;

    virtual unique_ptr<BaseTrackingCalibrationWidget> trackingCalibrationWidget() const = 0;
    virtual bool calibrateTracking(const QVector<EyesPosition> &map) = 0;

    virtual bool startTracking() = 0;
    virtual bool stopTracking() = 0;

signals:
    virtual void eyesPositionChanged(const EyesPosition &pos) = 0;
};

Q_DECLARE_INTERFACE(BaseEyetrackerPlugin, "br.ufba.dcc.eyecommunicator-cepred.BaseEyetrackerPlugin")

#endif // BASEEYETRACKERPLUGIN_HPP
