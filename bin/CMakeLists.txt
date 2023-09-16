# CMAKE Modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# Dependencies
find_package(pntr REQUIRED)
find_package(pntr_app REQUIRED)

# Iterate through each .c file
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(APPEND example_sources ${sources})

# Do each example
foreach (example_source ${example_sources})
    # Create the basename for the example
    get_filename_component(example_name ${example_source} NAME)

    set(example_source_source ${example_source})
    string(REPLACE ".c" "" example_name ${example_name})

    # raylib
    if (PNTR_PEANUTGB_BUILD_BIN_RAYLIB)
        find_package(raylib QUIET)
        set(example_name_raylib ${example_name}_raylib)

        add_executable(${example_name_raylib}
            ${example_source_source}
        )
        target_link_libraries(${example_name_raylib} PUBLIC
            raylib
            pntr
            pntr_app
            pntr_peanutgb
        )
        target_compile_definitions(${example_name_raylib} PUBLIC PNTR_APP_RAYLIB)
    endif()

    # SDL
    if (PNTR_PEANUTGB_BUILD_BIN_SDL)
        find_package(SDL2 QUIET)
        set(example_name_sdl ${example_name}_sdl)

        # SDL
        set(SDL_SHARED FALSE)
        set(SDL2_SHARED FALSE)
        set(SDL_STATIC TRUE)
        set(SDL_TEST FALSE)
        set(SDL_TESTS FALSE)
        set(INTERFACE_SDL2_SHARED false)
        set(SDL2_DISABLE_UNINSTALL TRUE)
        set(SDL2_DISABLE_INSTALL TRUE)
        set(SDL_INSTALL_TESTS FALSE)
        find_package(SDL2 REQUIRED)
        set(SDL2_LIBRARIES SDL2-static)
        #set(SDL2_LIBRARIES SDL2::SDL2 SDL2::SDL2main)

        add_executable(${example_name_sdl}
            ${example_source_source}
        )
        target_link_libraries(${example_name_sdl} PUBLIC
            ${SDL2_LIBRARIES}
            pntr
            pntr_app
            pntr_peanutgb
        )
        target_compile_definitions(${example_name_sdl} PUBLIC PNTR_APP_SDL)
    endif()

    # CLI
    if (PNTR_PEANUTGB_BUILD_BIN_CLI AND NOT EMSCRIPTEN)
        set(example_name_cli ${example_name}_cli)

        add_executable(${example_name_cli}
            ${example_source_source}
        )
        target_link_libraries(${example_name_cli} PUBLIC
            pntr
            pntr_app
            pntr_peanutgb
        )
        target_compile_definitions(${example_name_cli} PUBLIC PNTR_APP_CLI)
        set_property(TARGET ${example_name_cli} PROPERTY C_STANDARD 99)
    endif()

endforeach ()

# Resources
#file(GLOB resources resources/*)
#set(examples_resources)
#list(APPEND examples_resources ${resources})
#file(COPY ${examples_resources} DESTINATION "resources/")
