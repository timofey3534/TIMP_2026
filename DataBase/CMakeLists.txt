project(DataBase)

add_executable(DataBase
    main.cpp
)

target_link_libraries(DataBase PRIVATE
    Qt5::Core
    Qt5::Sql
)
