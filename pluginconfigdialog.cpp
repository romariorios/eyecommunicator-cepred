#include "pluginconfigdialog.hpp"
#include "ui_pluginconfigdialog.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

PluginConfigDialog::PluginConfigDialog(
    const QVector<BaseEyetrackerPlugin::Param> params,
    QWidget *parent
) :
    QDialog(parent),
    ui(new Ui::PluginConfigDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Eyetracker parameters"));

    auto &l = *ui->mainLayout;

    for (auto p : params) {
        auto fieldLayout = new QHBoxLayout;
        l.addLayout(fieldLayout);

        QWidget *paramWidget = nullptr;

        fieldLayout->addWidget(new QLabel{p.fullName, this});
        switch (p.type) {
        case BaseEyetrackerPlugin::Param::Int:
        {
            auto spinBox = new QSpinBox{this};
            paramWidget = spinBox;
            spinBox->setRange(p.values[0].toInt(), p.values[1].toInt());
            fieldLayout->addWidget(spinBox);
            break;
        }
        case BaseEyetrackerPlugin::Param::Float:
        {
            auto spinBox = new QDoubleSpinBox{this};
            paramWidget = spinBox;
            spinBox->setRange(p.values[0].toInt(), p.values[1].toInt());
            fieldLayout->addWidget(spinBox);
            break;
        }
        case BaseEyetrackerPlugin::Param::String:
            paramWidget = new QLineEdit{this};
            fieldLayout->addWidget(paramWidget);
            break;
        case BaseEyetrackerPlugin::Param::Set:
        {
            auto comboBox = new QComboBox{this};
            paramWidget = comboBox;
            for (auto value : p.values) {
                comboBox->addItem(value.toString());
            }

            fieldLayout->addWidget(comboBox);
            break;
        }
        default:
            paramWidget = new QLabel{tr("(invalid)"), this};
            fieldLayout->addWidget(paramWidget);
        }

        _fields[p.name] = { paramWidget, p.type };
    }
}

PluginConfigDialog::~PluginConfigDialog()
{
    delete ui;
}

#include <QDebug>

void PluginConfigDialog::accept()
{
    for (auto f : _fields) {
        qDebug() << f;
    }

    return QDialog::accept();
}
