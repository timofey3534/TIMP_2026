project(Singleton)

add_executable(Singleton
    main.cpp
    singleton_classic.h
    singleton_safe.h
)

target_link_libraries(Singleton PRIVATE
    Qt5::Core
)
