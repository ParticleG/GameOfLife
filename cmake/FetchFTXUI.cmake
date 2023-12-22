include(FetchContent)

FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ParticleG/FTXUI
        GIT_TAG main
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()