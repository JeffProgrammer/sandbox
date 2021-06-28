cmake_minimum_required(VERSION 3.10)
project(GraphicsSandbox)

function(advanced_option_off)
    foreach(arg IN LISTS ARGN)
        option(${arg} "" OFF)
        mark_as_advanced(FORCE ${arg})
    endforeach()
endfunction()

set(CMAKE_CXX_STANDARD 14)

advanced_option_off(GLFW_BUILD_DOCS GLFW_BUILD_EXAMPLES GLFW_BUILD_TESTS GLFW_USE_OSMESA GLFW_VULKAN_STATIC GLFW_INSTALL BUILD_SHARED_LIBS)

# Third Party
add_subdirectory(thirdparty/glfw)

add_library(glad thirdparty/glad/src/glad.c)

add_library(imgui 
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_widgets.cpp

    thirdparty/imgui/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
)

# Our Source Code
set(SANDBOX_SRC
    src/core/camera.h
    src/core/camera.cc

    src/app.h
    src/app.cc
    src/main.cc

    src/apps/main/mainApp.h
    src/apps/main/mainApp.cc

    src/apps/cube/cubeApp.h
    src/apps/cube/cubeApp.cc

    src/apps/cpuParticlesApp/cpuParticlesApp.h
    src/apps/cpuParticlesApp/cpuParticlesApp.cc
)

add_executable(sandbox ${SANDBOX_SRC})
target_link_libraries(sandbox glfw glad imgui)
target_include_directories(sandbox PRIVATE src)

# OpenGL Library
if (APPLE)
    target_link_libraries(sandbox "-framework OpenGL")
elseif (WIN32)
    target_link_libraries(sandbox OpenGL32)
elseif (UNIX)
    target_link_libraries(sandbox GL)
endif()

include_directories(
    thirdparty/glfw/include
    thirdparty/glm
    thirdparty/glad/include
    thirdparty/imgui
)

add_definitions(
    -DGLFW_STATIC 
    -DGLFW_INCLUDE_NONE
    -DIMGUI_IMPL_OPENGL_LOADER_GLAD
)