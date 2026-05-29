# Проект по дисциплине «Технологии и методы программирования»

**Учебная группа:** 251-354  
**Репозиторий:** [timofey3534/TIMP_2026](https://github.com/timofey3534/TIMP_2026)  
**Wiki:** [Документация проекта](https://github.com/timofey3534/TIMP_2026/wiki)

---

## Состав команды

| Участник | Роль |
|:---|:---|
| **Досмаев Илья** | Team Lead / DevOps |
| **Галушкин Тимофей** | Backend Developer |
| **Елейви Мухаммад** | Frontend Developer |
| **Лысков Денис** | Support |

---

## Вариант задания (Вариант 25)

| Алгоритм | Реализация |
|:---|:---|
| **AES-128-CBC** | Симметричное шифрование/дешифрование (OpenSSL EVP) |
| **SHA-384** | Хеширование (Qt `QCryptographicHash`) |
| **Метод хорд** | Решение f(x) = x³ − x − 2 = 0 |
| **Стеганография** | LSB внедрение текста в PNG (`QImage`) |

---

## Структура репозитория

```
TIMP_2026/
├── EchoServer/               # Qt TCP-сервер (порт 33333)
│   ├── EchoServer.pro
│   ├── main.cpp
│   ├── mytcpserver.h/.cpp    # TCP-сервер (QTcpServer)
│   ├── functionsforserver.h/.cpp  # Все 4 алгоритма
│   └── dataBase.h            # Singleton SQLite-логгер
├── Client/                   # Qt Widgets GUI клиент
│   ├── Client.pro
│   ├── mainwindow.h/.cpp     # Главное окно (4 вкладки)
│   └── connectiondialog.h/.cpp  # Диалог подключения
├── UnitTests/                # Модульные тесты (QtTest)
│   ├── UnitTests.pro
│   └── tst_funcforserver_test.cpp
├── DataBase/                 # Демо работы с SQLite
├── Singleton/                # Три реализации паттерна Singleton
│   ├── singleton_classic.h   # Классическая (ручное удаление)
│   └── singleton_safe.h      # С Destroyer + Meyers C++11
├── docs/diagrams/            # UML диаграммы (PlantUML)
│   ├── usecase.puml          # UseCase диаграмма
│   └── classdiagram.puml     # Диаграмма классов
├── Dockerfile                # Multi-stage build
└── docker-compose.yml
```

---

## Быстрый старт (Docker)

```bash
git clone https://github.com/timofey3534/TIMP_2026.git
cd TIMP_2026
docker compose up --build
```

Тест:
```bash
echo "SHA384:hello" | nc -q1 localhost 33333
echo "CHORD:1:2:1e-9" | nc -q1 localhost 33333
```

---

## Протокол сервера (порт 33333)

| Команда | Описание |
|:---|:---|
| `SHA384:<данные>` | SHA-384 хеш |
| `AES_ENCRYPT:<ключ>:<текст>` | Шифрование AES-128-CBC → hex |
| `AES_DECRYPT:<ключ>:<hex>` | Дешифрование |
| `CHORD:<a>:<b>:<eps>` | Корень f(x)=x³−x−2 на [a,b] |
| `STEG_EMBED:<src>:<dst>:<msg>` | Внедрить сообщение в PNG |
| `STEG_EXTRACT:<path>` | Извлечь сообщение из PNG |

Неизвестная команда → `error`

---

## Сборка вручную (Linux / WSL)

```bash
# Зависимости
sudo apt install qtbase5-dev qt5-qmake libqt5sql5-sqlite libqt5gui5 libssl-dev build-essential

# Сервер
cd EchoServer && qmake EchoServer.pro && make && ./EchoServer

# GUI клиент
cd Client && qmake Client.pro && make && ./Client

# Unit-тесты
cd UnitTests && qmake UnitTests.pro && make && ./UnitTests
```
