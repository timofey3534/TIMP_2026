project(EchoServer)

add_executable(EchoServer
    main.cpp
    mytcpserver.cpp
    mytcpserver.h
    functionsforserver.cpp
    functionsforserver.h
    dataBase.h
)

target_link_libraries(EchoServer PRIVATE
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Qt5::Gui          # нужен для QImage (стеганография)
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_compile_definitions(EchoServer PRIVATE QT_DEPRECATED_WARNINGS)
