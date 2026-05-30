cmake_minimum_required(VERSION 3.16)
project(TIMP_2026 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt auto-generation (MOC, UIC, RCC) — включаем глобально
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ── Подсказка для CLion: укажи путь к Qt в Settings → CMake → CMake options:
#    -DCMAKE_PREFIX_PATH=C:/Qt/5.15.2/msvc2019_64
#    или на Linux:  /usr/lib/x86_64-linux-gnu/cmake
# ──────────────────────────────────────────────────────────────────────────────

find_package(Qt5 COMPONENTS Core Network Sql Gui Widgets Test REQUIRED)
find_package(OpenSSL REQUIRED)

add_subdirectory(EchoServer)
add_subdirectory(Client)
add_subdirectory(UnitTests)
add_subdirectory(DataBase)
add_subdirectory(Singleton)
