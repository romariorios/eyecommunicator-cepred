#include "pluginconfigdialog.hpp"
#include "ui_pluginconfigdialog.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QToolButton>

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
            spinBox->setValue(p.defaultValue.toInt());
            fieldLayout->addWidget(spinBox);
            break;
        }
        case BaseEyetrackerPlugin::Param::Float:
        {
            auto spinBox = new QDoubleSpinBox{this};
            paramWidget = spinBox;
            spinBox->setRange(p.values[0].toFloat(), p.values[1].toFloat());
            spinBox->setValue(p.defaultValue.toFloat());
            fieldLayout->addWidget(spinBox);
            break;
        }
        case BaseEyetrackerPlugin::Param::String:
        {
            auto lineEdit = new QLineEdit{this};
            paramWidget = lineEdit;
            lineEdit->setText(p.defaultValue.toString());
            fieldLayout->addWidget(lineEdit);
            break;
        }
        case BaseEyetrackerPlugin::Param::Path:
        {
            auto path = new QLineEdit{this};
            path->setText(p.defaultValue.toString());
            path->setEnabled(false);
            fieldLayout->addWidget(path);

            auto search = new QToolButton{this};
            search->setText("...");
            connect(search, &QToolButton::clicked, [path, p, this]()
            {
                path->setText(
                    QFileDialog::getOpenFileName(
                        this, p.fullName));
            });
            fieldLayout->addWidget(search);

            paramWidget = path;
            break;
        }
        case BaseEyetrackerPlugin::Param::Set:
        {
            auto comboBox = new QComboBox{this};
            paramWidget = comboBox;
            for (auto value : p.values) {
                comboBox->addItem(value.toString());
            }
            comboBox->setCurrentIndex(p.defaultValue.toInt());

            fieldLayout->addWidget(comboBox);
            break;
        }
        default:
            paramWidget = new QLabel{tr("(invalid)"), this};
            fieldLayout->addWidget(paramWidget);
        }

        _fields[p.name] = paramWidget;
    }
}

PluginConfigDialog::~PluginConfigDialog()
{
    delete ui;
}

#include <QDebug>

void PluginConfigDialog::accept()
{
    for (auto k : _fields.keys()) {
        auto f = _fields[k];
        QVariant val;

        auto spinBox = qobject_cast<QSpinBox *>(f);
        if (spinBox)
            val = spinBox->value();

        auto doubleSpinBox = qobject_cast<QDoubleSpinBox *>(f);
        if (doubleSpinBox)
            val = doubleSpinBox->value();

        auto lineEdit = qobject_cast<QLineEdit *>(f);
        if (lineEdit)
            val = lineEdit->text();

        auto comboBox = qobject_cast<QComboBox *>(f);
        if (comboBox)
            val = comboBox->currentIndex();

        _result[k] = val;
    }

    return QDialog::accept();
}
