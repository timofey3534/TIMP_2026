#include <QCoreApplication>
#include <QDebug>

#include "singleton_classic.h"
#include "singleton_safe.h"

// Определения статических членов для классической реализации
SingletonClassic* SingletonClassic::p_instance = nullptr;

// Определения для улучшённой реализации
SingletonSafe*     SingletonSafe::p_instance = nullptr;
SingletonDestroyer SingletonSafe::destroyer;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Классический Singleton — ручное удаление
    SingletonClassic* s1 = SingletonClassic::getInstance();
    SingletonClassic* s2 = SingletonClassic::getInstance();
    qDebug() << "Classic: same instance?" << (s1 == s2);  // true
    delete SingletonClassic::getInstance();                // ручное удаление

    // Улучшённый Singleton — авто-удаление через Destroyer
    SingletonSafe* safe1 = SingletonSafe::getInstance();
    SingletonSafe* safe2 = SingletonSafe::getInstance();
    qDebug() << "Safe:    same instance?" << (safe1 == safe2);  // true

    // Meyers Singleton — современный C++11, потокобезопасный
    SingletonModern& m1 = SingletonModern::getInstance();
    SingletonModern& m2 = SingletonModern::getInstance();
    qDebug() << "Modern:  same instance?" << (&m1 == &m2);  // true

    return 0;
}
