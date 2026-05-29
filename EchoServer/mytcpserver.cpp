#include "mytcpserver.h"
#include "functionsforserver.h"
#include "dataBase.h"
#include <QDebug>
#include <QHostAddress>

MyTcpServer::~MyTcpServer()
{
    mTcpServer->close();
}

MyTcpServer::MyTcpServer(QObject* parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);

    DataBase::instance().open();

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if (!mTcpServer->listen(QHostAddress::Any, 33333)) {
        qDebug() << "server is not started";
    } else {
        qDebug() << "server is started on port 33333";
    }
}

void MyTcpServer::slotNewConnection()
{
    mTcpSocket = mTcpServer->nextPendingConnection();
    m_buffer.clear();

    mTcpSocket->write(
        "TIMP Server ready.\r\n"
        "Commands: SHA384:<data>  AES_ENCRYPT:<key>:<text>  AES_DECRYPT:<key>:<hex>\r\n"
        "          CHORD:<a>:<b>:<eps>  STEG_EMBED:<src>:<dst>:<msg>  STEG_EXTRACT:<path>\r\n"
    );

    connect(mTcpSocket, &QTcpSocket::readyRead,
            this, &MyTcpServer::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected,
            this, &MyTcpServer::slotClientDisconnected);
}

void MyTcpServer::slotServerRead()
{
    m_buffer.append(mTcpSocket->readAll());

    // Process every complete line (terminated by \n or \x01)
    while (true) {
        int nlIdx  = m_buffer.indexOf('\n');
        int delIdx = m_buffer.indexOf('\x01');

        int idx = -1;
        if      (nlIdx  != -1 && delIdx != -1) idx = qMin(nlIdx, delIdx);
        else if (nlIdx  != -1)                 idx = nlIdx;
        else if (delIdx != -1)                 idx = delIdx;
        else break;

        const QString command = QString::fromUtf8(m_buffer.left(idx)).trimmed();
        m_buffer.remove(0, idx + 1);

        if (command.isEmpty()) continue;

        qDebug() << ">> " << command;
        const QString result = parsing(command);
        qDebug() << "<< " << result;

        DataBase::instance().log(command, result);
        mTcpSocket->write((result + "\r\n").toUtf8());
    }
}

void MyTcpServer::slotClientDisconnected()
{
    m_buffer.clear();
    mTcpSocket->close();
}
