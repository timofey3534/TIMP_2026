#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QSpinBox;
QT_END_NAMESPACE

class ConnectionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectionDialog(QWidget* parent = nullptr);

    QString host() const;
    int     port() const;

private:
    QLineEdit* m_hostEdit;
    QSpinBox*  m_portSpin;
};

#endif // CONNECTIONDIALOG_H
