# Find ImGUI
# Use ImGUI as an external project
#
###
SET(NAME ImGUI)
SET(URL "https://github.com/ocornut/imgui.git")
SET(${NAME}_INSTALL_DIR  ${CMAKE_BINARY_DIR}/external/${NAME})
IF(NOT imgui_DOWNLOADED)
    find_package(Git)
    include(FindPythonInterp)
    file(MAKE_DIRECTORY ${${NAME}_INSTALL_DIR})
    execute_process(
            COMMAND ${GIT_EXECUTABLE} clone ${URL} ${NAME}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external
    )
    SET(imgui_DOWNLOADED 1 CACHE STRING "Set to 1 if imgui is found, 0 otherwise")
ENDIF()

SET(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
FIND_PACKAGE(gl3w REQUIRED)
find_package(glfw3 REQUIRED)
find_package( Threads REQUIRED)

add_library(imgui STATIC
    ${gl3w_SOURCE}
    ${ImGUI_INSTALL_DIR}/imgui.cpp
    ${ImGUI_INSTALL_DIR}/imgui_demo.cpp
    ${ImGUI_INSTALL_DIR}/imgui_draw.cpp
    ${ImGUI_INSTALL_DIR}/imgui_widgets.cpp
    ${ImGUI_INSTALL_DIR}/examples/imgui_impl_glfw.cpp
    ${ImGUI_INSTALL_DIR}/examples/imgui_impl_opengl3.cpp
)

target_link_libraries(imgui
    ${OPENGL_LIBRARIES}
    glfw
    dl
        ${CMAKE_THREAD_LIBS_INIT}
)
target_include_directories(imgui
        PUBLIC ${gl3w_INCLUDE_DIRS}
        PUBLIC ${ImGUI_INSTALL_DIR}
        PUBLIC ${ImGUI_INSTALL_DIR}/examples
)
target_compile_definitions(imgui PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GL3W)