cmake_minimum_required(VERSION 3.30)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(WisE LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(Lua REQUIRED)
find_package(sol2 REQUIRED)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tools/imgui)

set(IMGUI_FILES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

add_executable(prog)
target_sources(prog
    PRIVATE
        source/main.cpp
        source/headers.cpp
        ${IMGUI_FILES}
)

target_sources(prog
    PRIVATE
        FILE_SET CXX_MODULES FILES
        source/modules/window.cppm
        source/modules/engine.cppm
        source/modules/time.cppm
        source/modules/camera.cppm
        source/modules/luaConfigs.cppm
)

target_include_directories(prog PRIVATE
    /usr/local/include
    ${Vulkan_INCLUDE_DIRS}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${LUA_INCLUDE_DIR}
)

target_compile_definitions(prog PRIVATE
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_ENABLE_EXPERIMENTAL
)

if(MSVC)
  target_compile_options(prog PRIVATE /W4)
else()
  target_compile_options(prog PRIVATE -Wall -Wextra)
endif()

target_compile_options(prog PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-fmodules-ts>
)

target_link_libraries(prog PRIVATE
    Vulkan::Vulkan
    SDL3::SDL3
    SDL3_image::SDL3_image
    ${LUA_LIBRARIES}
    sol2::sol2
)

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        ${CMAKE_CURRENT_BINARY_DIR}/data

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/lua
        ${CMAKE_CURRENT_BINARY_DIR}/lua
)

add_dependencies(prog copy_assets)
