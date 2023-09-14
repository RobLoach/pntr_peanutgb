cmake_minimum_required(VERSION 3.11)
project(pntr_peanutgb
    DESCRIPTION "pntr_peanutgb: Peanut-GB for pntr"
    HOMEPAGE_URL "https://github.com/robloach/pntr_peanutgb"
    VERSION 0.1.0
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

option(PNTR_PEANUTGB_BUILD_BIN "pntr_peanutgb: bin" ${PNTR_PEANUTGB_IS_MAIN})
option(PNTR_PEANUTGB_BUILD_BIN_RAYLIB "pntr_peanutgb: raylib" ${PNTR_PEANUTGB_IS_MAIN})
option(PNTR_PEANUTGB_BUILD_BIN_CLI "pntr_peanutgb: CLI" ${PNTR_PEANUTGB_IS_MAIN})

if (PNTR_PEANUTGB_BUILD_BIN)
    add_subdirectory(bin)
endif()
