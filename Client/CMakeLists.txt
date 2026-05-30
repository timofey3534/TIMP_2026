project(Client)

add_executable(Client
    main.cpp
    mainwindow.cpp
    mainwindow.h
    connectiondialog.cpp
    connectiondialog.h
)

target_link_libraries(Client PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
)
