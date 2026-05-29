#include "mainwindow.h"
#include "connectiondialog.h"

#include <QTcpSocket>
#include <QTabWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QSplitter>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle("TIMP 2026 — Client");
    setMinimumSize(700, 550);
    buildUi();
}

// ─── UI construction ──────────────────────────────────────────────────────────

void MainWindow::buildUi()
{
    // Menu bar
    auto* menuFile = menuBar()->addMenu("&Файл");
    auto* actConnect = menuFile->addAction("&Подключиться...");
    auto* actDisconn = menuFile->addAction("&Отключиться");
    auto* actQuit    = menuFile->addAction("&Выход");
    connect(actConnect, &QAction::triggered, this, &MainWindow::onConnectClicked);
    connect(actDisconn, &QAction::triggered, this, [this]{ if(m_socket) m_socket->disconnectFromHost(); });
    connect(actQuit,    &QAction::triggered, this, &QWidget::close);

    auto* menuHelp = menuBar()->addMenu("&Справка");
    menuHelp->addAction("О программе", this, [this]{
        QMessageBox::about(this, "О программе",
            "<b>TIMP 2026 Client</b><br>"
            "Клиент для TIMP TCP-сервера.<br><br>"
            "Алгоритмы: AES-128-CBC, SHA-384,<br>"
            "Метод хорд, Стеганография (LSB).");
    });

    // Central splitter: tabs | log
    auto* splitter = new QSplitter(Qt::Vertical, this);

    auto* tabs = new QTabWidget(splitter);
    tabs->addTab(buildAesTab(),           "AES");
    tabs->addTab(buildHashTab(),          "SHA-384");
    tabs->addTab(buildChordTab(),         "Метод хорд");
    tabs->addTab(buildSteganographyTab(), "Стеганография");

    auto* logGroup = new QGroupBox("Лог команд и ответов сервера", splitter);
    m_log = new QTextEdit(logGroup);
    m_log->setReadOnly(true);
    m_log->setFont(QFont("Courier", 9));
    auto* logLayout = new QVBoxLayout(logGroup);
    logLayout->addWidget(m_log);

    splitter->addWidget(tabs);
    splitter->addWidget(logGroup);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    setCentralWidget(splitter);

    // Status bar
    m_statusLabel = new QLabel("Не подключён");
    statusBar()->addPermanentWidget(m_statusLabel);

    // Connect button in status bar
    auto* connectBtn = new QPushButton("Подключиться");
    statusBar()->addWidget(connectBtn);
    connect(connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
}

QWidget* MainWindow::buildAesTab()
{
    auto* w = new QWidget;
    auto* layout = new QVBoxLayout(w);

    auto* keyGroup = new QGroupBox("Ключ шифрования", w);
    m_aesKey = new QLineEdit("secretkey", keyGroup);
    auto* kg = new QVBoxLayout(keyGroup);
    kg->addWidget(new QLabel("Ключ (до 16 символов, дополняется нулями):"));
    kg->addWidget(m_aesKey);

    auto* ioGroup = new QGroupBox("Данные", w);
    m_aesInput  = new QLineEdit(ioGroup);
    m_aesInput->setPlaceholderText("Текст для шифрования / hex для расшифрования");
    m_aesOutput = new QLineEdit(ioGroup);
    m_aesOutput->setReadOnly(true);
    m_aesOutput->setPlaceholderText("Результат");

    auto* btnEnc = new QPushButton("Зашифровать →");
    auto* btnDec = new QPushButton("← Расшифровать");
    auto* btnRow = new QHBoxLayout;
    btnRow->addWidget(btnEnc);
    btnRow->addWidget(btnDec);

    auto* iog = new QVBoxLayout(ioGroup);
    iog->addWidget(new QLabel("Входные данные:"));
    iog->addWidget(m_aesInput);
    iog->addLayout(btnRow);
    iog->addWidget(new QLabel("Результат:"));
    iog->addWidget(m_aesOutput);

    layout->addWidget(keyGroup);
    layout->addWidget(ioGroup);
    layout->addStretch();

    connect(btnEnc, &QPushButton::clicked, this, &MainWindow::onAesEncrypt);
    connect(btnDec, &QPushButton::clicked, this, &MainWindow::onAesDecrypt);
    return w;
}

QWidget* MainWindow::buildHashTab()
{
    auto* w = new QWidget;
    auto* layout = new QVBoxLayout(w);

    m_shaInput = new QLineEdit(w);
    m_shaInput->setPlaceholderText("Введите строку для хеширования");

    m_shaOutput = new QTextEdit(w);
    m_shaOutput->setReadOnly(true);
    m_shaOutput->setMaximumHeight(70);
    m_shaOutput->setFont(QFont("Courier", 9));
    m_shaOutput->setPlaceholderText("SHA-384 хеш (96 hex символов)");

    auto* btn = new QPushButton("Вычислить SHA-384");

    layout->addWidget(new QLabel("Строка:"));
    layout->addWidget(m_shaInput);
    layout->addWidget(btn);
    layout->addWidget(new QLabel("Хеш (SHA-384):"));
    layout->addWidget(m_shaOutput);
    layout->addStretch();

    connect(btn, &QPushButton::clicked, this, &MainWindow::onSha384);
    return w;
}

QWidget* MainWindow::buildChordTab()
{
    auto* w = new QWidget;
    auto* layout = new QVBoxLayout(w);

    auto* info = new QLabel(
        "<b>Уравнение:</b> f(x) = x³ − x − 2 = 0<br>"
        "Корень на интервале [1, 2] ≈ 1.5213797");
    info->setTextFormat(Qt::RichText);

    auto* form = new QFormLayout;
    m_chordA   = new QDoubleSpinBox; m_chordA->setRange(-1e6, 1e6); m_chordA->setValue(1.0);
    m_chordB   = new QDoubleSpinBox; m_chordB->setRange(-1e6, 1e6); m_chordB->setValue(2.0);
    m_chordEps = new QDoubleSpinBox;
    m_chordEps->setDecimals(10);
    m_chordEps->setRange(1e-12, 1.0);
    m_chordEps->setValue(1e-7);
    m_chordEps->setSingleStep(1e-7);

    form->addRow("a (левая граница):",  m_chordA);
    form->addRow("b (правая граница):", m_chordB);
    form->addRow("ε (точность):",       m_chordEps);

    m_chordOut = new QLineEdit;
    m_chordOut->setReadOnly(true);
    m_chordOut->setPlaceholderText("Приближённый корень");
    m_chordOut->setFont(QFont("Courier", 10));

    auto* btn = new QPushButton("Найти корень (метод хорд)");

    layout->addWidget(info);
    layout->addSpacing(8);
    layout->addLayout(form);
    layout->addWidget(btn);
    layout->addWidget(new QLabel("Результат:"));
    layout->addWidget(m_chordOut);
    layout->addStretch();

    connect(btn, &QPushButton::clicked, this, &MainWindow::onChord);
    return w;
}

QWidget* MainWindow::buildSteganographyTab()
{
    auto* w = new QWidget;
    auto* layout = new QVBoxLayout(w);

    // Embed section
    auto* embedGroup = new QGroupBox("Внедрение сообщения (STEG_EMBED)", w);
    m_stegSrcPath = new QLineEdit;  m_stegSrcPath->setPlaceholderText("/путь/к/исходному.png");
    m_stegDstPath = new QLineEdit;  m_stegDstPath->setPlaceholderText("/путь/к/результату.png");
    m_stegMessage = new QLineEdit;  m_stegMessage->setPlaceholderText("Секретное сообщение");

    auto* btnSrc = new QPushButton("...");
    auto* btnDst = new QPushButton("...");
    auto* srcRow = new QHBoxLayout; srcRow->addWidget(m_stegSrcPath); srcRow->addWidget(btnSrc);
    auto* dstRow = new QHBoxLayout; dstRow->addWidget(m_stegDstPath); dstRow->addWidget(btnDst);

    auto* btnEmbed = new QPushButton("Внедрить сообщение в изображение");

    auto* eg = new QVBoxLayout(embedGroup);
    eg->addWidget(new QLabel("Исходное изображение:")); eg->addLayout(srcRow);
    eg->addWidget(new QLabel("Файл назначения:"));      eg->addLayout(dstRow);
    eg->addWidget(new QLabel("Сообщение:"));            eg->addWidget(m_stegMessage);
    eg->addWidget(btnEmbed);

    // Extract section
    auto* extractGroup = new QGroupBox("Извлечение сообщения (STEG_EXTRACT)", w);
    m_stegExtPath = new QLineEdit;  m_stegExtPath->setPlaceholderText("/путь/к/изображению.png");
    auto* btnExtSrc = new QPushButton("...");
    auto* extRow = new QHBoxLayout; extRow->addWidget(m_stegExtPath); extRow->addWidget(btnExtSrc);
    auto* btnExtract = new QPushButton("Извлечь сообщение из изображения");

    m_stegOut = new QTextEdit;
    m_stegOut->setReadOnly(true);
    m_stegOut->setMaximumHeight(60);

    auto* xg = new QVBoxLayout(extractGroup);
    xg->addWidget(new QLabel("Изображение со скрытым сообщением:"));
    xg->addLayout(extRow);
    xg->addWidget(btnExtract);
    xg->addWidget(new QLabel("Извлечённое сообщение:"));
    xg->addWidget(m_stegOut);

    layout->addWidget(embedGroup);
    layout->addWidget(extractGroup);
    layout->addStretch();

    // File chooser buttons
    connect(btnSrc,    &QPushButton::clicked, this, [this]{
        QString f = QFileDialog::getOpenFileName(this, "Исходное изображение", "", "PNG (*.png);;All (*.*)");
        if (!f.isEmpty()) m_stegSrcPath->setText(f);
    });
    connect(btnDst,    &QPushButton::clicked, this, [this]{
        QString f = QFileDialog::getSaveFileName(this, "Сохранить как", "", "PNG (*.png)");
        if (!f.isEmpty()) m_stegDstPath->setText(f);
    });
    connect(btnExtSrc, &QPushButton::clicked, this, [this]{
        QString f = QFileDialog::getOpenFileName(this, "Изображение", "", "PNG (*.png);;All (*.*)");
        if (!f.isEmpty()) m_stegExtPath->setText(f);
    });

    connect(btnEmbed,   &QPushButton::clicked, this, &MainWindow::onSteganographyEmbed);
    connect(btnExtract, &QPushButton::clicked, this, &MainWindow::onSteganographyExtract);
    return w;
}

// ─── Connection ───────────────────────────────────────────────────────────────

void MainWindow::onConnectClicked()
{
    ConnectionDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    if (!m_socket) {
        m_socket = new QTcpSocket(this);
        connect(m_socket, &QTcpSocket::connected,    this, &MainWindow::onConnected);
        connect(m_socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
        connect(m_socket, &QTcpSocket::readyRead,    this, &MainWindow::onReadyRead);
        connect(m_socket, &QAbstractSocket::errorOccurred, this, &MainWindow::onSocketError);
    }
    appendLog(QString("[%1] Подключение к %2:%3...")
              .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
              .arg(dlg.host()).arg(dlg.port()));
    m_socket->connectToHost(dlg.host(), static_cast<quint16>(dlg.port()));
}

void MainWindow::onConnected()
{
    m_statusLabel->setText(QString("Подключён: %1:%2")
        .arg(m_socket->peerAddress().toString())
        .arg(m_socket->peerPort()));
    appendLog("[OK] Подключение установлено.");
}

void MainWindow::onDisconnected()
{
    m_statusLabel->setText("Не подключён");
    appendLog("[--] Соединение закрыто.");
    m_buffer.clear();
}

void MainWindow::onSocketError()
{
    appendLog("[ERR] " + m_socket->errorString());
}

void MainWindow::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    while (m_buffer.contains('\n')) {
        int idx = m_buffer.indexOf('\n');
        QString line = QString::fromUtf8(m_buffer.left(idx)).trimmed();
        m_buffer.remove(0, idx + 1);

        if (!line.isEmpty()) {
            appendLog("[<<] " + line);

            // Route response to the appropriate output widget
            if (m_pendingCmd.startsWith("AES_ENCRYPT") || m_pendingCmd.startsWith("AES_DECRYPT"))
                m_aesOutput->setText(line);
            else if (m_pendingCmd.startsWith("SHA384"))
                m_shaOutput->setPlainText(line);
            else if (m_pendingCmd.startsWith("CHORD"))
                m_chordOut->setText(line);
            else if (m_pendingCmd.startsWith("STEG"))
                m_stegOut->setPlainText(line);

            m_pendingCmd.clear();
        }
    }
}

// ─── Command sending ──────────────────────────────────────────────────────────

void MainWindow::sendCommand(const QString& cmd)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "Нет подключения", "Сначала подключитесь к серверу.");
        return;
    }
    m_pendingCmd = cmd;
    appendLog("[>>] " + cmd);
    m_socket->write((cmd + "\n").toUtf8());
}

void MainWindow::appendLog(const QString& msg)
{
    m_log->append(msg);
}

// ─── Tab slots ────────────────────────────────────────────────────────────────

void MainWindow::onAesEncrypt()
{
    sendCommand(QString("AES_ENCRYPT:%1:%2").arg(m_aesKey->text(), m_aesInput->text()));
}

void MainWindow::onAesDecrypt()
{
    sendCommand(QString("AES_DECRYPT:%1:%2").arg(m_aesKey->text(), m_aesInput->text()));
}

void MainWindow::onSha384()
{
    sendCommand("SHA384:" + m_shaInput->text());
}

void MainWindow::onChord()
{
    sendCommand(QString("CHORD:%1:%2:%3")
                .arg(m_chordA->value())
                .arg(m_chordB->value())
                .arg(m_chordEps->value()));
}

void MainWindow::onSteganographyEmbed()
{
    sendCommand(QString("STEG_EMBED:%1:%2:%3")
                .arg(m_stegSrcPath->text())
                .arg(m_stegDstPath->text())
                .arg(m_stegMessage->text()));
}

void MainWindow::onSteganographyExtract()
{
    sendCommand("STEG_EXTRACT:" + m_stegExtPath->text());
}
