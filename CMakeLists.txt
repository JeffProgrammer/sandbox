cmake_minimum_required(VERSION 3.10)
project(GraphicsSandbox)

add_subdirectory(thirdparty/glfw)

set(SANDBOX_SRC
    src/app.h
    src/app.cc
    src/main.cc

    src/sandbox.cc
)

add_executable(sandbox ${SANDBOX_SRC})
target_link_libraries(sandbox glfw)

if (APPLE)
    target_link_libraries(sandbox "-framework OpenGL")
endif()

include_directories(
    thirdparty/glfw/include
    thirdparty/glm
)

add_definitions(-DGLFW_STATIC)

function(advanced_option_off)
    foreach(arg IN LISTS ARGN)
        set(${arg} OFF)
        mark_as_advanced(FORCE ${arg})
    endforeach()
endfunction()

advanced_option_off(GLFW_BUILD_DOCS GLFW_BUILD_EXAMPLES GLFW_BUILD_TESTS GLFW_USE_OSMESA GLFW_VULKAN_STATIC GLFW_INSTALL BUILD_SHARED_LIBS)