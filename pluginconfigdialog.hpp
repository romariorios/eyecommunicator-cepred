#ifndef PLUGINCONFIGDIALOG_HPP
#define PLUGINCONFIGDIALOG_HPP

#include <QDialog>

#include "baseeyetrackerplugin.hpp"

namespace Ui {
class PluginConfigDialog;
}

class PluginConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginConfigDialog(
        const QVector<BaseEyetrackerPlugin::Param> params,
        QWidget *parent = 0);
    ~PluginConfigDialog();

    inline QVariantHash result() const { return _result; }

public slots:
    void accept();

private:
    Ui::PluginConfigDialog *ui;
    QHash<QString, QWidget *> _fields;
    QVariantHash _result;
};

#endif // PLUGINCONFIGDIALOG_HPP
