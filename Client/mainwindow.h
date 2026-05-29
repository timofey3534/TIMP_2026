#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>

QT_BEGIN_NAMESPACE
class QTcpSocket;
class QTabWidget;
class QTextEdit;
class QLabel;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QPushButton;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onConnectClicked();
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError();

    // AES tab
    void onAesEncrypt();
    void onAesDecrypt();
    // SHA tab
    void onSha384();
    // Chord tab
    void onChord();
    // Steganography tab
    void onSteganographyEmbed();
    void onSteganographyExtract();

private:
    void buildUi();
    QWidget* buildAesTab();
    QWidget* buildHashTab();
    QWidget* buildChordTab();
    QWidget* buildSteganographyTab();

    void sendCommand(const QString& cmd);
    void appendLog(const QString& msg);

    // Network
    QTcpSocket* m_socket = nullptr;
    QByteArray  m_buffer;

    // Status bar label
    QLabel* m_statusLabel = nullptr;

    // Log widget (shared across tabs)
    QTextEdit* m_log = nullptr;

    // AES widgets
    QLineEdit* m_aesKey      = nullptr;
    QLineEdit* m_aesInput    = nullptr;
    QLineEdit* m_aesOutput   = nullptr;

    // SHA widgets
    QLineEdit* m_shaInput    = nullptr;
    QTextEdit* m_shaOutput   = nullptr;

    // Chord widgets
    QDoubleSpinBox* m_chordA   = nullptr;
    QDoubleSpinBox* m_chordB   = nullptr;
    QDoubleSpinBox* m_chordEps = nullptr;
    QLineEdit*      m_chordOut = nullptr;

    // Steganography widgets
    QLineEdit* m_stegSrcPath  = nullptr;
    QLineEdit* m_stegDstPath  = nullptr;
    QLineEdit* m_stegMessage  = nullptr;
    QLineEdit* m_stegExtPath  = nullptr;
    QTextEdit* m_stegOut      = nullptr;

    // Pending command (to match async response)
    QString m_pendingCmd;
};

#endif // MAINWINDOW_H
