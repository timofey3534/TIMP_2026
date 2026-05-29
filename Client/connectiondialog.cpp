#include "connectiondialog.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QLabel>

ConnectionDialog::ConnectionDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Подключение к серверу");
    setMinimumWidth(300);

    m_hostEdit = new QLineEdit("localhost", this);
    m_portSpin = new QSpinBox(this);
    m_portSpin->setRange(1, 65535);
    m_portSpin->setValue(33333);

    auto* form = new QFormLayout;
    form->addRow("Хост:", m_hostEdit);
    form->addRow("Порт:", m_portSpin);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Параметры подключения к TIMP-серверу:"));
    layout->addLayout(form);
    layout->addWidget(buttons);
}

QString ConnectionDialog::host() const { return m_hostEdit->text().trimmed(); }
int     ConnectionDialog::port() const { return m_portSpin->value(); }
