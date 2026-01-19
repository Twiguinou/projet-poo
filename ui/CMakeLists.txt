find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Quick QuickControls2)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(projet-poo-ui main.cpp main.qrc)
target_link_libraries(projet-poo-ui PRIVATE projet-poo-libs Qt6::Core Qt6::Gui Qt6::QuickControls2)
