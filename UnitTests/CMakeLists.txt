project(UnitTests)

add_executable(UnitTests
    tst_funcforserver_test.cpp
    # Shared sources from EchoServer
    ../EchoServer/functionsforserver.cpp
    ../EchoServer/mytcpserver.cpp
)

# Нужно, чтобы находил functionsforserver.h и dataBase.h
target_include_directories(UnitTests PRIVATE
    ../EchoServer
)

target_link_libraries(UnitTests PRIVATE
    Qt5::Core
    Qt5::Test
    Qt5::Sql
    Qt5::Network
    Qt5::Gui
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Регистрируем как CTest-тест
enable_testing()
add_test(NAME UnitTests COMMAND UnitTests)
