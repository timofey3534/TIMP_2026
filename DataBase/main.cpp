#include <QCoreApplication>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Создаём базу данных типа QSQLITE (встроенный драйвер Qt).
    // Другие драйверы: QPSQL (PostgreSQL), QMYSQL (MySQL), QODBC ...
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Test.db");

    if (!db.open()) {
        qDebug() << db.lastError().text();
        return 1;
    }

    QSqlQuery query(db);

    // Создание таблицы (если ещё не существует).
    // Поля: login VARCHAR(20), password VARCHAR(20)
    query.exec("CREATE TABLE IF NOT EXISTS User("
               "login    VARCHAR(20) NOT NULL, "
               "password VARCHAR(20) NOT NULL"
               ")");

    // Вставка записей с использованием именованных placeholder-ов.
    auto insertUser = [&](const QString& login, const QString& password) {
        query.prepare("INSERT INTO User(login, password) "
                      "VALUES (:login, :password)");
        query.bindValue(":login",    login);
        query.bindValue(":password", password);
        if (!query.exec())
            qDebug() << "Insert error:" << query.lastError().text();
    };

    insertUser("admin", "123");
    insertUser("user",  "qwerty");

    // Выборка всех записей.
    query.exec("SELECT * FROM User");

    QSqlRecord rec              = query.record();
    const int  loginIndex       = rec.indexOf("login");
    const int  passwordIndex    = rec.indexOf("password");

    qDebug() << "=== Users ===";
    while (query.next()) {
        qDebug() << query.value(loginIndex).toString()
                 << "\t" << query.value(passwordIndex).toString();
    }

    // Удаление таблицы (раскомментировать при необходимости).
    // query.exec("DROP TABLE User");

    db.close();
    return 0;
}
