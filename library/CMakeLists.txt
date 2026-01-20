add_subdirectory(nlohmann-json)

add_library(projet-poo-libs
    Adherent.h
    Bibliotheque.h
    Database.cpp
    Database.h
    Livre.h
    Table.cpp
    Table.h
)
target_include_directories(projet-poo-libs PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(projet-poo-libs PUBLIC nlohmann_json::nlohmann_json)
