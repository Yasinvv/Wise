cmake_minimum_required(VERSION 3.22)
project(VulkanSDL3App LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

set(SRC_FILES
    source/main.cpp
)

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
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(prog ${SRC_FILES} ${IMGUI_FILES})

target_include_directories(prog PRIVATE
    /usr/local/include
    ${Vulkan_INCLUDE_DIRS}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_BINARY_DIR}/data
    COMMENT "Copying assets to build directory..."
)
add_dependencies(prog copy_assets)

target_compile_definitions(prog PRIVATE VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)

if(MSVC)
  target_compile_options(prog PRIVATE /W4)
else()
  target_compile_options(prog PRIVATE -Wall -Wextra)
endif()

target_link_libraries(prog PRIVATE
    Vulkan::Vulkan
    SDL3::SDL3
    SDL3_image::SDL3_image
)


if(UNIX AND NOT APPLE)
  set_target_properties(prog PROPERTIES
        INSTALL_RPATH "/usr/local/lib64;${Vulkan_LIBRARY_DIRS}"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()
