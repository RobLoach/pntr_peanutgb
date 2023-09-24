cmake_minimum_required(VERSION 3.11)
project(pntr_peanutgb
    DESCRIPTION "pntr_peanutgb"
    HOMEPAGE_URL "https://github.com/robloach/pntr_peanutgb"
    VERSION 0.0.1
    LANGUAGES C
)

# pntr_peanutgb
add_library(pntr_peanutgb INTERFACE)
target_include_directories(pntr_peanutgb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(PNTR_PEANUTGB_IS_MAIN TRUE)
else()
    set(PNTR_PEANUTGB_IS_MAIN FALSE)
endif()

option(PNTR_PEANUTGB_BUILD "Build pntr_peanutgb" ${PNTR_PEANUTGB_IS_MAIN})

if (PNTR_PEANUTGB_BUILD)
    option(SDL "Build SDL" TRUE)
    option(RAYLIB "Build raylib" TRUE)
    option(LIBRETRO "Build libretro" TRUE)
    option(CLI "Build CLI" TRUE)
    option(WEB "Build Web" TRUE)

    set(SOURCES
        bin/pntr_peanutgb.c
    )

    # Have pntr_app_starter build the project
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/.cmake")
    find_package(pntr_app_starter)
endif()
