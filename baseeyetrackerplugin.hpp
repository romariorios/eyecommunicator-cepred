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
        enum Type {
            Invalid = 0,
            Int,
            Float,
            String,
            Set
        } type;
        QVector<QVariant> values;
        QVariant defaultValue;
    };

    virtual ~BaseEyetrackerPlugin() {}

    virtual QVector<Param> availableTrackingParams() const = 0;
    virtual bool setTrackingParams(const QVariantHash &params) = 0;

    virtual unique_ptr<BaseTrackingCalibrationWidget> trackingCalibrationWidget() const = 0;
    virtual bool calibrateTracking(const QVector<EyesPosition> &map) = 0;

    virtual bool startTracking() = 0;
    virtual bool stopTracking() = 0;

    inline const QObject *qObject() const { return dynamic_cast<const QObject *>(this); }

signals:
    virtual void eyesPositionChanged(const EyesPosition &pos) = 0;
};

Q_DECLARE_INTERFACE(BaseEyetrackerPlugin, "br.ufba.dcc.eyecommunicator-cepred.BaseEyetrackerPlugin")

#define EYETRACKER_PLUGIN_INTERFACE(className)\
    Q_INTERFACES(BaseEyetrackerPlugin)\
\
public:\
    className##EyetrackerPlugin();\
    ~className##EyetrackerPlugin();\
\
    QVector<Param> availableTrackingParams() const;\
    bool setTrackingParams(const QVariantHash &params);\
\
    unique_ptr<BaseTrackingCalibrationWidget> trackingCalibrationWidget() const;\
    bool calibrateTracking(const QVector<EyesPosition> &map);\
\
    bool startTracking();\
    bool stopTracking();

#endif // BASEEYETRACKERPLUGIN_HPP
