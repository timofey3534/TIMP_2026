# Проект по дисциплине «Технологии и методы программирования»

**Учебная группа:** 251-354  
**Репозиторий:** [timofey3534/TIMP_2026](https://github.com/timofey3534/TIMP_2026)

---

## Состав команды

| Участник | Роль |
|:---|:---|
| **Досмаев Илья** | Team Lead / DevOps |
| **Галушкин Тимофей** | Backend Developer |
| **Елейви Мухаммад** | Frontend Developer |
| **Лысков Денис** | Support |

---

## Техническое задание (Вариант 25)

Программный комплекс на базе Qt TCP-сервера, реализующий:

| Алгоритм | Описание |
|:---|:---|
| **AES-128-CBC** | Симметричное шифрование / дешифрование (OpenSSL) |
| **SHA-384** | Хеширование данных (Qt `QCryptographicHash`) |
| **Метод хорд** | Решение нелинейного уравнения f(x) = x³ − x − 2 |
| **Стеганография** | Внедрение текстового сообщения в PNG-изображение (LSB) |

---

## Структура репозитория

```
TIMP_2026/
├── EchoServer/               # Qt-проект сервера
│   ├── EchoServer.pro
│   ├── main.cpp
│   ├── mytcpserver.h/.cpp    # TCP-сервер (Qt)
│   ├── functionsforserver.h/.cpp  # реализация алгоритмов
│   └── dataBase.h            # Singleton-обёртка над SQLite
├── UnitTests/                # Модульные тесты (QtTest)
│   ├── UnitTests.pro
│   └── tst_funcforserver_test.cpp
├── Dockerfile
├── docker-compose.yml
└── README.md
```

---

## Протокол сервера

Сервер слушает порт **33333**. Команды отправляются строками (разделитель `\n` или `\x01`).

| Команда | Пример |
|:---|:---|
| `SHA384:<данные>` | `SHA384:hello` |
| `AES_ENCRYPT:<ключ>:<текст>` | `AES_ENCRYPT:mykey:hello world` |
| `AES_DECRYPT:<ключ>:<hex>` | `AES_DECRYPT:mykey:3a7b...` |
| `CHORD:<a>:<b>:<eps>` | `CHORD:1:2:0.0001` |
| `STEG_EMBED:<src>:<dst>:<сообщение>` | `STEG_EMBED:/in.png:/out.png:secret` |
| `STEG_EXTRACT:<путь>` | `STEG_EXTRACT:/out.png` |

Неизвестная команда → `error`.

---

## Запуск через Docker

```bash
docker compose up --build
```

Тест подключения:

```bash
echo -e "SHA384:hello\n" | nc localhost 33333
```

---

## Сборка локально (Linux / WSL)

```bash
sudo apt install qtbase5-dev qt5-qmake libqt5sql5-sqlite libssl-dev
cd EchoServer
qmake EchoServer.pro && make
./EchoServer
```

---

## Запуск unit-тестов

```bash
cd UnitTests
qmake UnitTests.pro && make && ./UnitTests
```
