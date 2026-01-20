find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
set(CMAKE_AUTOMOC ON)

qt_add_executable(projet-poo-ui
    main.cpp
    MainWindow.h
    MainWindow.cpp
)
target_link_libraries(projet-poo-ui PRIVATE projet-poo-libs Qt6::Widgets)
