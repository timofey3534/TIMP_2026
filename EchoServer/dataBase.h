#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>

// Singleton database wrapper (Meyers singleton — no manual memory management).
class DataBase
{
public:
    static DataBase& instance() {
        static DataBase db;
        return db;
    }

    bool open(const QString& path = "server.db") {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "server_connection");
        m_db.setDatabaseName(path);
        if (!m_db.open()) {
            qDebug() << "DataBase::open failed:" << m_db.lastError().text();
            return false;
        }
        QSqlQuery q(m_db);
        q.exec("CREATE TABLE IF NOT EXISTS logs ("
               "id        INTEGER PRIMARY KEY AUTOINCREMENT, "
               "command   TEXT, "
               "result    TEXT, "
               "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)");
        m_opened = true;
        return true;
    }

    void log(const QString& command, const QString& result) {
        if (!m_opened) return;
        QSqlQuery q(m_db);
        q.prepare("INSERT INTO logs(command, result) VALUES(:cmd, :res)");
        q.bindValue(":cmd", command);
        q.bindValue(":res", result);
        if (!q.exec())
            qDebug() << "DataBase::log error:" << q.lastError().text();
    }

private:
    DataBase() : m_opened(false) {}
    DataBase(const DataBase&)            = delete;
    DataBase& operator=(const DataBase&) = delete;

    QSqlDatabase m_db;
    bool         m_opened;
};

#endif // DATABASE_H
