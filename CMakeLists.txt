cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)
project(GraphicsSandbox)

function(advanced_option_off)
    foreach(arg IN LISTS ARGN)
        option(${arg} "" OFF)
        mark_as_advanced(FORCE ${arg})
    endforeach()
endfunction()

function(add_app)
    foreach(arg IN LISTS ARGN)
        file(GLOB_RECURSE files_found RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/apps/${arg}/*.*")
        list(APPEND apps ${files_found})
    endforeach()

    set(SANDBOX_SRC "${SANDBOX_SRC};${apps}" PARENT_SCOPE)
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
    src/core/cube.h

    src/gfx/gfxCmdBuffer.h
    src/gfx/gfxCmdBuffer.cc
    src/gfx/gfxDevice.h
    src/gfx/gfxDevice.cc
    src/gfx/gfxTypes.h

    src/app.h
    src/app.cc
    src/main.cc

    src/apps/main/mainApp.h
    src/apps/main/mainApp.cc
)

if (APPLE)
    set(SANDBOX_SRC
        ${SANDBOX_SRC}

        src/gfx/Metal/gfxMetalDevice.h
        src/gfx/Metal/gfxMetalDevice.mm
    )
else()
    set(SANDBOX_SRC
        ${SANDBOX_SRC}

        src/gl/gpuProfiler.h
        src/gl/gpuProfiler.cc
        src/gl/shader.h
        src/gl/shader.cc

        src/gfx/OpenGL/gfxGLCmdBuffer.h
        src/gfx/OpenGL/gfxGLCmdBuffer.cc
        src/gfx/OpenGL/gfxGLDevice.h
        src/gfx/OpenGL/gfxGLDevice.cc
    )
endif()

add_app(01_Hello_Cubes 02_Cpu_Particles 03_Draw_Performance 04_Forward_Rendering)

add_executable(sandbox ${SANDBOX_SRC})
target_link_libraries(sandbox glfw glad imgui)
target_include_directories(sandbox PRIVATE src)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${SANDBOX_SRC})

if (MSVC)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sandbox)
    set_property(TARGET sandbox PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/")
elseif (XCODE)
    set_target_properties(sandbox PROPERTIES 
        XCODE_GENERATE_SCHEME TRUE
        XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/"
    )
endif()

if (APPLE)
    target_link_libraries(sandbox "-framework Metal")
elseif (WIN32)
    target_link_libraries(sandbox OpenGL32)
elseif (UNIX)
    target_link_libraries(sandbox GL)
endif()

if (NOT APPLE)
    include_directories(thirdparty/glad/include)
endif()
include_directories(
    thirdparty/glfw/include
    thirdparty/glm
    thirdparty/imgui
)

add_definitions(
    -DGLFW_STATIC 
    -DGLFW_INCLUDE_NONE
    -DIMGUI_IMPL_OPENGL_LOADER_GLAD
)

if (APPLE)
    add_definitions(-DGFX_METAL)
else()
    add_definitions(-DGFX_OPENGL)
endif()