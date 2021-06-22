cmake_minimum_required(VERSION 3.10)
project(GraphicsSandbox)

function(advanced_option_off)
    foreach(arg IN LISTS ARGN)
        option(${arg} "" OFF)
        mark_as_advanced(FORCE ${arg})
    endforeach()
endfunction()

advanced_option_off(GLFW_BUILD_DOCS GLFW_BUILD_EXAMPLES GLFW_BUILD_TESTS GLFW_USE_OSMESA GLFW_VULKAN_STATIC GLFW_INSTALL BUILD_SHARED_LIBS)

# Third Party
add_subdirectory(thirdparty/glfw)

add_library(glad thirdparty/glad/src/glad.c)

# Our Source Code
set(SANDBOX_SRC
    src/app.h
    src/app.cc
    src/main.cc

    src/sandbox.cc
)

add_executable(sandbox ${SANDBOX_SRC})
target_link_libraries(sandbox glfw glad)

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
)

add_definitions(-DGLFW_STATIC -DGLFW_INCLUDE_NONE)